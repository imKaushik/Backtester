//
// Created by Kaushik on 07/09/23.
//
#include "XYAlgorithm.h"

/*
 * Trade Price = Avg Price in the execution interval +
 *               X% of Avg Spread in the execution interval
 */

Price XYAlgorithm::calculateTradePrice(SecurityId security_id, Time start_time, Time end_time,
                                       const MarketDataManager &market_data_manager) {
    double cum_price = 0, cum_spread = 0;
    int count = 0;
    MinuteBarRecord minute_record;

    // Iterate over each 1 minute interval
    for (Time curr_time = start_time; curr_time <= end_time; curr_time += NANOSECONDS_PER_MINUTE) {
        if (!market_data_manager.getMarketData(security_id, curr_time, minute_record)) {
            LOG("[XYAlgorithm] Not fatal, just skip this interval");
            continue;
        }
        cum_price += minute_record.price;
        cum_spread += minute_record.spread;
        count++;
    }
    return count == 0 ? 0 : cum_price / count + X * ((cum_spread / count) / 100);
}

/*
 * Process one target position request using market data and push any fulfilled orders
 * to Fills using FillManager.
 *
 * Uses the following to decide fills:
 * Trade Price = Avg Price in the execution interval +
 *               X% of Avg Spread in the execution interval
 *
 * Trade Volume is at max Y% of the volume tarded in any minute interval
 *
 * Specifics:
 * Say I have arrival time for req at 02:10:04 and end time at 02:15:56.
 * The execution interval for this is 02:11:00 to 02:16:00.
 * For avg price/spread calculation, and deciding how much quantity we can get,
 * we will use 02:11:00, 02:12:00, 02:13:00, 02:14:00, 02:15:00 and 02:16:00's data.
 *
 * To calculate fill_time, we always use the execution interval's border time,
 * say I can get quantity q in 02:10:04 to 02:11:00, I mark 02:11:00 at the time of
 * buy and fill time as = 02:11:00 - 02:10:04 = 56 sec. Obviously all the time
 * notations are going to be in epochs in the calculation.
 *
 * Note : Observed in market data that each request is atleast 10 minutes long. So
 * don't have to worry about cases of arrival time and end time in the same interval.
 * Even thought it shouldn't matter for our calculation.
 *
 */
bool XYAlgorithm::execute(const TargetPositionRecord &target_record,
                          const MarketDataManager &market_data_manager,
                          FillManager &fill_manager,
                          const PortfolioManager &portfolio_manager) {
    SecurityId security_id = target_record.security_id;

    // If target_left negative, you have to sell, else you have to buy.
    Quantity target_left = target_record.target_position -
                           portfolio_manager.getLatestPosition(target_record.security_id);
    int sign = target_left < 0 ? -1 : 1;
    target_left = abs(target_left);

    Time start_time = roundOff(target_record.arrival_time);
    Time end_time = roundOff(target_record.end_time);

    Price trade_price = calculateTradePrice(security_id, start_time, end_time, market_data_manager);

    // Process the volume info and calculate the fills.
    // I would love to package the below in a separate function, but too many variables
    // needed from context.
    MinuteBarRecord minute_record;

    // Iterate over each 1 minute interval
    for (Time curr_time = start_time; curr_time <= end_time; curr_time += NANOSECONDS_PER_MINUTE) {
        if (!market_data_manager.getMarketData(security_id, curr_time, minute_record)) {
            LOG("[XYAlgorithm] Not fatal, just skip this interval");
            continue;
        }

        // Calculate how much quantity you can get in this interval.
        Quantity fill_quantity = fmin(target_left, Y * minute_record.volume / 100);

        // If you get anything, push it to fills.
        if (fill_quantity > 0) {
            Time fill_time = curr_time - target_record.arrival_time;
            if (!fill_manager.addFill(Fill(security_id,
                                           curr_time - target_record.arrival_time,
                                           curr_time,
                                           sign * fill_quantity,
                                           trade_price))) {
                return false;
            }

            // Update how much left in this request.
            target_left -= fill_quantity;
            if (target_left == 0) break;
        }
    }

    return true;
}