//
// Created by Kaushik on 06/09/23.
//

#ifndef CUBIST_PORTFOLIOSNAPSHOT_H
#define CUBIST_PORTFOLIOSNAPSHOT_H
#include "DataModel.h"
#include "backtest_util.h"

/*
 * Represents PortfolioSnapshot - cash and position of security at any time.
 * A really passive object, just stores some data, and minimal support operations on it.
 * Internal usages only, used by the Security class.
 */

class PortfolioSnapshot : public DataModel
{
public:
    PortfolioSnapshot() : cash(0), position(0), timestamp(0) {};
    PortfolioSnapshot(Price cash, Quantity position,
                      Time timestamp) : cash(cash), position(position), timestamp(timestamp) {};
    Price cash;
    Quantity position;
    Time timestamp; //representing end of minute this is a representation of.

    // Get string representation
    virtual std::string str() const;

    // Reset all data members.
    virtual void clear();

    // Get the timestamp associated with this object.
    virtual Time getTime() const;

    // Given a CSVRow, populate the data for the object from it.
    virtual bool parseCSVRow(char line[1024]);

};
#endif //CUBIST_PORTFOLIOSNAPSHOT_H
