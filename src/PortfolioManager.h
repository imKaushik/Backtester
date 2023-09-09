//
// Created by Kaushik on 07/09/23.
//

#ifndef CUBIST_PORTFOLIOMANAGER_H
#define CUBIST_PORTFOLIOMANAGER_H

#include "SecurityUniverse.h"

/*
 * Proxy Handler to portfolio data related information.
 * To abstract out the implementation details and restrict
 * the functions a user can do, built this proxy handler over the
 * SecurityUniverse.
 *
 * This class does not do any logical stuff, it just relays the operations
 * to Security Universe.
 */
class PortfolioManager {
public:
    // Fetches latest position of an instrument
    Quantity getLatestPosition(SecurityId) const;

    // Compute Pnls, dump and clean up portfolio data.
    void postProcess();
};

#endif //CUBIST_PORTFOLIOMANAGER_H
