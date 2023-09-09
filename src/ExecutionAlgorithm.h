//
// Created by Kaushik on 07/09/23.
//

#ifndef CUBIST_EXECUTIONALGORITHM_H
#define CUBIST_EXECUTIONALGORITHM_H

#include "TargetPositionRecord.h"
#include "MarketDataManager.h"
#include "FillManager.h"
#include "PortfolioManager.h"

/*
 * Algorithm Trading Logic Base Virtual Class :
 * Defines the basic interface of the components it needs to interact
 * with.
 *
 * The execute function should process a record, go over the market
 * intervals relevant for it, deccide a price and volume from the market
 * data, and send fills information. Notice only fills is non-const here.
 */
class ExecutionAlgorithm {
public:

    ExecutionAlgorithm() = default;

    virtual bool execute(const TargetPositionRecord &record,
                         const MarketDataManager &market_data_manager,
                         FillManager &fill_manager,
                         const PortfolioManager &portfolio_manager) = 0;

};

#endif //CUBIST_EXECUTIONALGORITHM_H
