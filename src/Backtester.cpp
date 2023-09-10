//
// Created by Kaushik on 08/09/23.
//
#include "Backtester.h"

/*
 * Backtester's core job is the following :
 * - Decide how to traverse the target position requests.
 * - Decide what data to load in market data manager.
 * - Hand off the processing of a request to strategy with appropriate market data.
 * - Clean up the objects after use.
 */

void Backtester::execute() {
    Week current_week;

    // Process below one week at a time.
    while (target_position_reader.hasNext()) {

        current_week = getWeek(target_position_reader.getNextTimestamp());

        market_data_manager.loadMarketData(current_week);

        std::vector<std::future<bool>> futures;

        while (target_position_reader.hasNext() &&
               getWeek(target_position_reader.getNextTimestamp()) == current_week) {
            // Process one record at a time.
            TargetPositionRecord record;
            target_position_reader.consumeRecord(record);

            // Preprocessing before submitting to algorithm,  read description of invoke_strategy
            cv_map[record.security_id][record.arrival_time];
            pq_map[record.security_id].push(record.arrival_time);

            futures.push_back(std::async(std::launch::async,
                                         std::bind(&Backtester::invoke_strategy, this, record, market_data_manager,
                                                   fill_manager, portfolio_manager)));
        }

        for (auto &future: futures) {
            future.get();
        }

        futures.clear();
        cv_map.clear();

        // Order of the following is important.
        portfolio_manager.postProcess();
        fill_manager.postProcess();
        market_data_manager.postProcess();
    }
}

/*
 * Submitting target records to the algorithm parallely using std::async,
 * (which mostly uses a thread pool). See how invoke_strategy is used above.
 * To ensure correct processing, we need the following:
 * - No two records of the same security should be processed at the same time.
 * - Records of the same security should be processed according to the arrival time.
 * This logic is implemented using condition variables and priority_queue for
 * the records submitted for processing with same security id.
 * 
 * For very small records, this might slow down the regular implementation.
 * For high number of records, with large number of instruments it is definitely going to help.
 */
bool Backtester::invoke_strategy(TargetPositionRecord &record, MarketDataManager &market_data_manager,
                                 FillManager &fill_manager, PortfolioManager &portfolio_manager) {
    SecurityId security_id = record.security_id;
    std::mutex m;
    std::unique_lock<std::mutex> lock(m);

    cv_map[security_id][record.arrival_time].wait(lock, [&]() {
        return !pq_map[security_id].empty() && record.arrival_time == pq_map[security_id].top();
    });

    // Critical section
    if (!algorithm->execute(record, market_data_manager,
                            fill_manager, portfolio_manager)) {
        ERROR("Failed to process the following target position request : " << record.str());
    } else {
        LOG("Processed target position record successfully : " << record.str());
    }

    pq_map[security_id].pop();
    m.unlock();

    cv_map[security_id].erase(record.arrival_time);
    if (!pq_map[security_id].empty()) {
        cv_map[security_id][pq_map[security_id].top()].notify_one();
    }

    return true;
}
