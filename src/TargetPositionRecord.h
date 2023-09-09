//
// Created by Kaushik on 04/09/23.
//

#ifndef CUBIST_TARGETPOSITIONRECORD_H
#define CUBIST_TARGETPOSITIONRECORD_H

#include "DataModel.h"
#include "backtest_util.h"

/*
 * Data record for Target Position Request : security id, arrival time, end time, target position
 * A really passive object, just stores some data, and minimal support operations on it.
 */

class TargetPositionRecord : public DataModel {
public:

    SecurityId security_id;
    Time arrival_time;
    Time end_time;
    double target_position;

    // Get string representation
    virtual std::string str() const;

    // Reset all data members.
    virtual void clear();

    // Get the timestamp associated with this object.
    virtual Time getTime() const;

    // Given a CSVRow, populate the data for the object from it.
    virtual bool parseCSVRow(char line[1024]);
};

#endif //CUBIST_TARGETPOSITIONRECORD_H
