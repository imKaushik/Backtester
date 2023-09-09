//
// Created by Kaushik on 07/09/23.
//

#ifndef CUBIST_XYALGORITHM_H
#define CUBIST_XYALGORITHM_H

#include "ExecutionAlgorithm.h"

/*
 * XYAlgorithm : Cleverly named because it just accepts X and Y as its input parameters. :)
 *
 * The execute function should process a record, go over the market
 * intervals relevant for it, decide a price and volume from the market
 * data, and send fills information. Notice only fills is non-const here.
 *
 * To iterate over multiple target position requests, just call algorithm.execute
 * multiple times. Trivial comment, but I just wanted to mention it.
 */
class XYAlgorithm : public ExecutionAlgorithm {
public:
    XYAlgorithm(double X, double Y) : X(X), Y(Y) {};

    // Process one target position request using market data and push any fulfilled orders
    // to Fills using FillManager.
    virtual bool execute(const TargetPositionRecord &target_record,
                         const MarketDataManager &market_data_manager,
                         FillManager &fill_manager,
                         const PortfolioManager &portfolio_manager);

private:
    // Calculates the trade price to be considered for orders.
    Price calculateTradePrice(SecurityId security_id, Time start_time, Time end_time,
                              const MarketDataManager &market_data_manager);

    double X;
    double Y;
};

#endif //CUBIST_XYALGORITHM_H
