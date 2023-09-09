//
// Created by Kaushik on 04/09/23.
//

#ifndef CUBIST_MINUTEBARRECORD_H
#define CUBIST_MINUTEBARRECORD_H

#include <string>
#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>

#include "DataModel.h"
#include "backtest_util.h"

/*
 * Data record for Minute bar information: storing price, spread, and volume in 1 minute intervals.
 * A really passive object, just stores some data, and minimal support operations on it.
 */
class MinuteBarRecord : public DataModel {

public:
    SecurityId security_id;
    Time start_time;
    Price price;
    Price spread;
    Quantity volume;

    MinuteBarRecord() : security_id(0), start_time(0), price(0),
                        spread(0), volume(0) {};

    MinuteBarRecord(SecurityId security_id, Time start_time, Price price,
                    Price spread, Quantity volume) :
            security_id(security_id), start_time(start_time),
            price(price), spread(spread), volume(volume) {};

    // Get string representation
    virtual std::string str() const;

    // Reset all data members.
    virtual void clear();

    // Get the timestamp associated with this object.
    virtual Time getTime() const;

    // Populate data members after parsing a CSV row.
    virtual bool parseCSVRow(char line[1024]);

};

#endif //CUBIST_MINUTEBARRECORD_H
