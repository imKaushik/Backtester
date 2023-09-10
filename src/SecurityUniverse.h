//
// Created by Kaushik on 04/09/23.
//

#ifndef CUBIST_SECURITYUNIVERSE_H
#define CUBIST_SECURITYUNIVERSE_H

#include "Security.h"
#include "backtest_util.h"
#include "CSVReader.h"

#include <filesystem>
#include <algorithm>
#include <execution>
#include <utility>
#include <unordered_map>

/*
 * The top reference to read any security data.
 *
 * In order to prevent unnecessary copies of this, making this a singleton class.
 * Any changes to add/delete securities or it's attributes are redirected by this.
 * But are allowed only by the proxy objects.
 *
 * Market Data Reader : Once it has a data source set, it can load market data from it.
 *
 * Most of the functions, simply check the records and pass it on to relevant security.
 *
 * Pnl Calculation : In order to calculate strategy level PnL, first compute pnl of all securities.
 *
 * Cleanup : weekly clean up market data/fill/pnl of all securities.
 */
class SecurityUniverse {

public:
    SecurityUniverse() = default;

    // Singleton interface
    static SecurityUniverse &getInstance() {
        static SecurityUniverse instance; // Create a single instance when called for the first time
        return instance;
    }

    // The directory within which I need to dump strategy pnl and fills information.
    void setOutputDir(const std::string& output_dir);

private:
    /* Market data related methods */
    void setDataSource(const std::string &file_name);

    bool getMarketData(SecurityId security_id, Time time, MinuteBarRecord &record) const;

    void loadMarketData(Week week) noexcept(false);

    void postProcessMarketData();

    /* Fill data related methods */

    bool addFill(Fill &&record);

    bool addFill(Fill &record);

    void postProcessFill();

    /* Portfolio - cash, position and pnl related methods */

    Quantity getLatestPosition(SecurityId security_id) const;

    void dumpStrategyPnl();

    void computeStrategyPnl();

    void postProcessPortfolio();

    // For now, no one needs to access this directly.
    Security *get(SecurityId security_id) const;

    // For singleton behaviour;
    SecurityUniverse(const SecurityUniverse &) = delete;

    SecurityUniverse &operator=(const SecurityUniverse &) = delete;

    // Attributes.
    Week week;
    mutable std::unordered_map<SecurityId, Security *> smap;
    std::unique_ptr<MarketDataReader> market_data_reader;
    WeekIntervalData<PnLRecord> strategy_pnl;
    std::string output_dir;

    // Can be only invoked by proxy objects.
    friend class MarketDataManager;
    friend class FillManager;
    friend class PortfolioManager;
};

#endif //CUBIST_SECURITYUNIVERSE_H
