//
// Created by Kaushik on 08/09/23.
//

#ifndef CUBIST_BACKTESTER_H
#define CUBIST_BACKTESTER_H

#include "MarketDataManager.h"
#include "FillManager.h"
#include "PortfolioManager.h"
#include "CSVReader.h"
#include "ExecutionAlgorithm.h"

#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <future>
#include <vector>
#include <queue>

/*
 * Backtester : The central block which joins data and the strategy.
 * It is given object references of the all the needed, without any implementation details.
 * So it's flexible to switch internal details of components, without changing this block.
 *
 * Backtester's core job is the following :
 * - Decide how to traverse the target position requests.
 * - Decide what data to load in market data manager.
 * - Hand off the processing of a request to strategy with appropriate market data.
 * - Clean up the objects after use.
 *
 *
 * Objects and their level calls from each.
 *  MarketDataManager : loadMarketData, getMarketData, postProcess
 *  FillManager : addFill, postProcess
 *  PortfolioManager : getLatestPosition, postProcess
 *  TargetPositionReader : consumeRecord
 *  Algorithm : handle a record from target position reader with the available market data,
 *              and make any fills data.
 */

class Backtester {
public:
    Backtester(MarketDataManager &market_data_manager,
               FillManager &fill_manager,
               PortfolioManager &portfolio_manager,
               const std::string &target_positions,
               ExecutionAlgorithm *algorithm) : market_data_manager(market_data_manager),
                                                fill_manager(fill_manager),
                                                portfolio_manager(portfolio_manager),
                                                target_position_reader(TargetPositionReader(target_positions)),
                                                algorithm(algorithm) {};

    // Processing a target position request, with available market data.
    void execute();

private:

 /*Submitting target records to the algorithm parallely using std::async,
 * (which mostly uses a thread pool).
 */

    bool invoke_strategy(TargetPositionRecord& record, MarketDataManager& market_data_manager, FillManager& fill_manager, PortfolioManager& portfolio_manager);

    MarketDataManager market_data_manager;
    FillManager fill_manager;
    PortfolioManager portfolio_manager;
    TargetPositionReader target_position_reader;
    ExecutionAlgorithm *algorithm;

    // For parallel implementation - explanation in cpp file.
    // Min Heap to store the pending requests associated with a security id, ordered by time.
    std::unordered_map<SecurityId, std::priority_queue<long long, std::vector<long long>, std::greater<long long>>> pq_map;
    // Map from SecurityId->[TimeStamp]->cv.
    std::unordered_map<SecurityId, std::unordered_map<Time, std::condition_variable>> cv_map;

};

#endif //CUBIST_BACKTESTER_H
