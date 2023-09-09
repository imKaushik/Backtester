//
// Created by Kaushik Gunda on 07/09/23.
//
#include "backtest_util.h"
#include "SecurityUniverse.h"

/* Market data related methods. */

// Set the data source for reading market data from.
void SecurityUniverse::setDataSource(const std::string &file_name) {
    market_data_reader = std::make_unique<MarketDataReader>(file_name);
}

// Get the market data in memory of the security at a particular time.
bool SecurityUniverse::getMarketData(SecurityId security_id, Time time, MinuteBarRecord &record) const {
    return get(security_id)->getMarketData(time, record);
}

// Load market data of all instruments of this week into memory.
void SecurityUniverse::loadMarketData(Week week) noexcept(false) {
    if (!market_data_reader) {
        throw std::runtime_error("Market data reader object is nullptr!");
    }

    this->week = week;
    fill_stream.open(output_dir + "/fills/" + std::to_string(week));

    Time next_time;
    MinuteBarRecord record;

    // If no records found for a week, it is not an error.
    int entries = 0;
    while (market_data_reader->hasNext()
           && getWeek(market_data_reader->getNextTimestamp()) <= week) {

        market_data_reader->consumeRecord(record);

        // If you are reading records prior to this week, skip them.
        if (getWeek(record.start_time) != week) continue;

        get(record.security_id)->addMarketData(record);
        entries++;
    }

    LOG("Loaded market data for week " << week << ", found " << entries << " records.");
}

// Clean up market info for all securities.
void SecurityUniverse::postProcessMarketData()
{
    for (auto& [security_id, security] : smap) {
        security->postProcessMarketData();
    }
}

/* Fill related methods*/

// Push a new fill record.
bool SecurityUniverse::addFill(Fill &&record) {
    if (fill_stream.is_open()) {
        fill_stream << record.str() << std::endl;
    }
    return get(record.security_id)->addFill(std::move(record));
}

// Push a new fill record.
bool SecurityUniverse::addFill(Fill &record) {
    // As and when you get a record, flush it to fill stream.
    if (fill_stream.is_open()) {
        fill_stream << record.str() << std::endl;
    }
    return get(record.security_id)->addFill(record);
}

// Clean up fills of all securities.
void SecurityUniverse::postProcessFill()
{
    // As and when you get a record, flush it to fill stream.
    for (auto& [security_id, security] : smap) {
        security->postProcessFill();
    }
    fill_stream.close();
}

// Security fetch method
Security* SecurityUniverse::get(SecurityId security_id) const {
    if (smap.find(security_id) == smap.end()) {
        smap[security_id] = new Security(security_id);
    }
    return smap[security_id];
}

void SecurityUniverse::setOutputDir(const std::string &output_dir) {
    this->output_dir = output_dir;
    std::filesystem::remove_all(output_dir);
    std::filesystem::create_directories(output_dir + "/fills/");
    std::filesystem::create_directories(output_dir + "/strategy_pnl/");
}

/* PnL and related methods*/

// Computes strategy PnL
void SecurityUniverse::computeStrategyPnl() {
    // Compute Every instrument's PnL
    for (auto security: smap) {
        security.second->computePnL();
    }

    // Compute Strategy PnL
    PnLRecord security_pnl;

    // Crude, but this can be made parallel in the future, so good.

    for (int day = START_DAY; day <= END_DAY; day++) {

        Time first_epoch = getFirstEpochOfDay(week, day) - EST_OFFSET;

        for (int minute = START_MINUTE; minute <= END_MINUTE; minute++) {
            Time curr_time = first_epoch + minute * NANOSECONDS_PER_MINUTE;

            Price final_pnl = 0;

            for (auto& [security_id, security] : smap) {
                // No need to check if it's invalid, we ensure every security
                // has pnl computed for every trading minute of the week.

                security->getPnL(curr_time, security_pnl);

                final_pnl += security_pnl.pnl;
            }
            strategy_pnl.insertRecord(curr_time, PnLRecord(final_pnl, curr_time));
        }
    }
}

// Fetch the latest position of a security.
Quantity SecurityUniverse::getLatestPosition(SecurityId security_id) const
{
    return get(security_id)->getLatestPosition();
}

void SecurityUniverse::dumpStrategyPnl()
{
    strategy_pnl.dump(output_dir + "/strategy_pnl/" + std::to_string(week));
}

// Compute PnL of the strategy, dump it and clear all related information.
void SecurityUniverse::postProcessPortfolio()
{
    computeStrategyPnl();
    dumpStrategyPnl();
    for (auto& [security_id, security] : smap) {
        security->postProcessPortfolio();
    }
    strategy_pnl.clear();
}
