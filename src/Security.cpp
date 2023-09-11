//
// Created by Kaushik on 07/09/23.
//
#include "Security.h"

/* Market Data related methods */
bool Security::getMarketData(Time time, MinuteBarRecord &r) const {
    return market_data.getRecord(time, r);
}

bool Security::addMarketData(MinuteBarRecord &record) {
    week = getWeek(record.start_time);
    if (market_data.insertRecord(record.start_time, record)) {
        LOG("[Security] Inserted minute bar data successfully : " << record.str());
    } else {
        std::cerr<<"[Security] Failed to insert minute bar data : " << record.str() << std::endl;
        return false;
    }
    return true;
}

bool Security::addMarketData(MinuteBarRecord &&record) {
    week = getWeek(record.start_time);
    if (market_data.insertRecord(record.start_time, std::move(record))) {
        LOG("[Security] Inserted minute bar data successfully : " << record.str());
    } else {
        std::cerr<<"[Security] Failed to insert minute bar data : " << record.str() << std::endl;
        return false;
    }
    return true;
}

void Security::postProcessMarketData()
{
    week = 0;
    market_data.clear();
}

/* Fill related methods */

// Push a fill record. Adjust cash, position related information too.
bool Security::addFill(Fill &record) {
    current_cash += -1 * record.fill_quantity * record.fill_price;
    current_position += record.fill_quantity;

    // Adjust and inset current_cash and current_position, will be useful for PnL
    if (fills.insertRecord(record.execute_time, record)) {
        LOG("[Security] Inserted the following fill successfully " << record.str());
        portfolio.insertRecord(record.execute_time,
                               PortfolioSnapshot(current_cash,
                                                 current_position, record.execute_time));
        LOG("[Security] Inserted the portfolio snapshot recorded associated successfully");
    } else {
        std::cerr << "[Security] Failed to add the following fill record " << record.str() << std::endl;
        return false;
    }
    return true;
}

// Push a fill record. Adjust cash, position related information too.
bool Security::addFill(Fill &&record) {
    current_cash += -1 * record.fill_quantity * record.fill_price;
    current_position += record.fill_quantity;

    // Adjust and inset current_cash and current_position, will be useful for PnL
    if (fills.insertRecord(record.execute_time, std::move(record))) {
        LOG("[Security] Inserted the following fill successfully " << record.str());
        portfolio.insertRecord(record.execute_time,
                               PortfolioSnapshot(current_cash,
                                                 current_position, record.execute_time));
        LOG("[Security] Inserted the portfolio snapshot recorded associated successfully");
    } else {
        std::cerr << "[Security]Failed to add the following fill record " << record.str() << std::endl;
        return false;
    }
    return true;
}

void Security::postProcessFill(const std::string& file_name)
{
    fills.dump(file_name);
    week = 0;
    fills.clear();
}

/* Portfolio - cash, position, pnl related methods*/
Quantity Security::getLatestPosition() const {
    return current_position;
}

bool Security::getPnL(Time t, PnLRecord &p) const {
    return pnl.getRecord(t, p);
}

void Security::computePnL() {
    Price curr_cash = 0;
    Quantity curr_position = 0;
    // Saved PnL from last week
    PnL curr_pnl = current_pnl;
    PortfolioSnapshot ps;
    MinuteBarRecord mbr;

    for (int day = START_DAY; day <= END_DAY; day++) {
        Time first_epoch = getFirstEpochOfDay(week, day) - EST_OFFSET;

        for (int minute = START_MINUTE; minute <= END_MINUTE; minute++) {

            Time curr_time = first_epoch + minute * NANOSECONDS_PER_MINUTE;

            if (!market_data.getRecord(curr_time, mbr)) {
                // If not market data, just used the last available pnl.
            } else if (!portfolio.getRecord(curr_time, ps)) {
                curr_pnl = curr_cash + curr_position * mbr.price;
            } else {
                // If there was a fill on that minute, update curr_cash and curr_position to that.
                curr_cash = ps.cash;
                curr_position = ps.position;
                curr_pnl = curr_cash + curr_position * mbr.price;
            }

            pnl.insertRecord(curr_time, PnLRecord(curr_pnl, curr_time));
        }
    }
    // Save to be used for next week
    current_pnl = curr_pnl;
}

// Clean up the cash, position and pnl information.
void Security::postProcessPortfolio()
{
    week = 0;
    portfolio.clear();
    // Dump security Pnl
    pnl.clear();
}