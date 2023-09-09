//
// Created by Kaushik on 06/09/23.
//

#ifndef CUBIST_FILL_H
#define CUBIST_FILL_H
#include "DataModel.h"
#include "backtest_util.h"

/*
 * Contains a fill record information.
 * A time based record, inherited from Data Model.
 * A really passive object, just stores some data, and minimal support operations on it.
 * Should ideally not have security_id as a member, should be abstracted out.
 */
class Fill : public DataModel {
public:

    Fill() : security_id(0), fill_time(0), execute_time(0),
             fill_quantity(0), fill_price(0) {};

    Fill(SecurityId security_id, Time fill_time,
         Time execute_time, Quantity fill_quantity, Price fill_price)
            : security_id(security_id), fill_time(fill_time), execute_time(execute_time),
              fill_quantity(fill_quantity), fill_price(fill_price) {};

    SecurityId security_id;
    Time fill_time;
    Time execute_time;
    Quantity fill_quantity;
    Price fill_price;

    // Fetch a string representation
    virtual std::string str() const;

    // Reset the data members.
    virtual void clear();

    // Get the timestamp associated
    virtual Time getTime() const;

    // Populate data members after parsing a CSV row
    virtual bool parseCSVRow(char line[1024]);

};

#endif //CUBIST_FILL_H
