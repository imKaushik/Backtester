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
    std::for_each(std::execution::par_unseq, smap.begin(), smap.end(),
		  [](const std::pair<SecurityId, Security*>& security)
		  { security.second->postProcessMarketData(); });
}

/* Fill related methods*/

// Push a new fill record.
bool SecurityUniverse::addFill(Fill &&record) {
    return get(record.security_id)->addFill(std::move(record));
}

// Push a new fill record.
bool SecurityUniverse::addFill(Fill &record) {
    return get(record.security_id)->addFill(record);
}

// Clean up fills of all securities.
void SecurityUniverse::postProcessFill()
{
    // As and when you get a record, flush it to fill stream.
	std::for_each(std::execution::par_unseq, smap.begin(), smap.end(),
			[this](const std::pair<SecurityId, Security*> &security)
			{ security.second->postProcessFill(this->output_dir + "/fills/" + std::to_string(security.first)); });
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
}

/* PnL and related methods*/

// Computes strategy PnL
void SecurityUniverse::computeStrategyPnl() {
    // Compute Every instrument's PnL
    std::for_each(std::execution::par_unseq, smap.begin(), smap.end(),
                  [](const std::pair<SecurityId, Security *> &security)
                  { security.second->computePnL(); });

    // Compute Strategy PnL


    for (int day = START_DAY; day <= END_DAY; day++) {

        Time first_epoch = getFirstEpochOfDay(week, day) - EST_OFFSET;

        for (int minute = START_MINUTE; minute <= END_MINUTE; minute++) {
            Time curr_time = first_epoch + minute * NANOSECONDS_PER_MINUTE;

            PnL final_pnl = 0;

            final_pnl = std::reduce(std::execution::par_unseq, smap.begin(), smap.end(), final_pnl,
                                        [curr_time](PnL accumulated, const auto &security) {
                                            PnLRecord security_pnl;
                                            security.second->getPnL(curr_time, security_pnl);
                                            return accumulated + security_pnl.pnl;
                                        });

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
    strategy_pnl.dump(output_dir + "/strategy_pnl");
}

// Compute PnL of the strategy, dump it and clear all related information.
void SecurityUniverse::postProcessPortfolio()
{
    computeStrategyPnl();
    dumpStrategyPnl();
    std::for_each(std::execution::par_unseq, smap.begin(), smap.end(),
		    [](const auto& security)
		    { security.second->postProcessPortfolio(); });
    strategy_pnl.clear();
}
