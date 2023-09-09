//
// Created by Kaushik on 07/09/23.
//

#ifndef CUBIST_FILLMANAGER_H
#define CUBIST_FILLMANAGER_H

#include "SecurityUniverse.h"
#include "Fill.h"

/*
 * Proxy Handler to fill data related information.
 * To abstract out the implementation details and restrict
 * the functions a user can do, built this proxy handler over the
 * SecurityUniverse.
 *
 * This class does not do any logical stuff, it just relays the operations
 * to Security Universe.
 */
class FillManager {
public:
    // Push a new fill record.
    bool addFill(Fill &);

    // Push a new fill record.
    bool addFill(Fill &&);

    // Clean up the fill records.
    void postProcess();

};

#endif //CUBIST_FILLMANAGER_H
