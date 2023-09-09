//
// Created by Kaushik on 08/09/23.
//

#ifndef CUBIST_PNLRECORD_H
#define CUBIST_PNLRECORD_H
#include "DataModel.h"
#include "backtest_util.h"

/*
 * Represents the pnl associated with a timestamp.
 * Could have replaced its usages with an long, but to be consistent
 * with time-series based data model objects, created an object.
 *
 * A really passive object, just stores some data, and minimal support operations on it.
 *
 * Internal usages only, used by the Security class.
 */
class PnLRecord : public DataModel
{
public:
    PnL pnl;
    Time timestamp;

    PnLRecord() : pnl(0), timestamp(0) {};

    PnLRecord(PnL pnl, Time timestamp) : pnl(pnl), timestamp(timestamp) {};

    // Get string representation
    virtual std::string str() const;

    // Reset all data members.
    virtual void clear();

    // Get the timestamp associated with this object.
    virtual Time getTime() const;

    // Given a CSVRow, populate the data for the object from it.
    virtual bool parseCSVRow(char line[1024]);

};
#endif //CUBIST_PNLRECORD_H
