//
// Created by Kaushik on 08/09/23.
//
#include "PortfolioManager.h"

// Fetches latest position of an instrument
Quantity PortfolioManager::getLatestPosition(SecurityId security_id) const {
    return SecurityUniverse::getInstance().getLatestPosition(security_id);
}

// Compute Pnls, dump and clean up portfolio data.
void PortfolioManager::postProcess() {
    SecurityUniverse::getInstance().postProcessPortfolio();
}