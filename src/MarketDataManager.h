//
// Created by Kaushik on 06/09/23.
//

#ifndef CUBIST_MARKETDATAMANAGER_H
#define CUBIST_MARKETDATAMANAGER_H

#include "backtest_util.h"
#include "SecurityUniverse.h"
#include "MinuteBarRecord.h"

/*
 * Proxy Handler to market data related information.
 * To abstract out the implementation details and restrict
 * the functions a user can do, built this proxy handler over the
 * SecurityUniverse.
 *
 * This class does not do any logical stuff, it just relays the operations
 * to Security Universe.
 */
class MarketDataManager
{
public:
    MarketDataManager() = delete;

    // The file name is necessary to create the market data reader.
    MarketDataManager(const std::string &file_name);

    // Load the market data related to this week into memory.
    void loadMarketData(Week week) noexcept(false);

    // Fetch the market data informaton for a security at a given time.
    bool getMarketData(SecurityId security_id, Time time, MinuteBarRecord& record) const;

    // Clean up the market data created for this week.
    void postProcess();
};
#endif //CUBIST_MARKETDATAMANAGER_H
