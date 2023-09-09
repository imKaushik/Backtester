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

        while (target_position_reader.hasNext() &&
               getWeek(target_position_reader.getNextTimestamp()) == current_week) {
            // Process one record at a time.
            TargetPositionRecord record;
            target_position_reader.consumeRecord(record);
            if (!algorithm->execute(record, market_data_manager,
                                    fill_manager, portfolio_manager)) {
                ERROR("Failed to process the following target position request : " << record.str());
            } else {
                LOG("Processed target position record successfully : " << record.str());
            }
        }

        // Order of the following is important.
        portfolio_manager.postProcess();
        fill_manager.postProcess();
        market_data_manager.postProcess();
    }
}