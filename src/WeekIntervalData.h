//
// Created by Kaushik on 06/09/23.
//

#ifndef CUBIST_WEEKINTERVALDATA_H
#define CUBIST_WEEKINTERVALDATA_H

#include <array>
#include <optional>
#include <iostream>
#include "backtest_util.h"

/*
 * WeekIntervalData : This template is tailored to store data efficiently
 * for every trading minute of a week i.e 9:30 AM to 5:00 PM EST.
 * This is 7.5 hours(=450 minutes) a day, 5 days a week.
 *
 * To insert/read objects of T, you need to provide a timestamp, which is
 * mapped to day of the week/minute of the day.
 *
 * The week for which the data is stored is decided based on the first
 * record inserted, any other writes after that are validated for correctness.
 *
 * Pre allocating T for every minute of the week, for faster access.
 * Should ideally be used only when you know your data is not sparse/need
 * for simpler time based access.
 *
 *
 */
template<typename T>
class WeekIntervalData {
public:
    static_assert(is_derived_from_data_model<T>::value, "Template typename class must be derived from Data Model.");

    /* Insert a record of type T associated with a timestamp.
     * The timestamp is broken down into
     * 1. Week number - to verify if this is the appropriate WeekInternvalData object to add to.
     * 2. Day of the week - which should be Monday to Friday, returns false if Sat/Sun.
     * 3. Minute of the day - which should be 9:30 AM to 4:00 PM EST, else return false.
     */

    bool insertRecord(Time timestamp, T &record);

    bool insertRecord(Time timestamp, T &&record);

    bool getRecord(Time timestamp, T &record) const;

    void clear();

    void dump(const std::string &file_name);

private:
    int validate(Time timestamp, bool ignore_week) const;

    std::array<std::optional<T>, WEEK_TRADING_MINUTES> data;
    mutable Time week;
};

// Insert a lvalue record, copy constructor invoked.
template<typename T>
bool WeekIntervalData<T>::insertRecord(Time timestamp, T &record) {
    int idx = validate(timestamp, true);
    if (idx == -1) {
        return false;
    }
    data[idx] = record;
    return true;
}

// Insert a rvalue record, move constructor invoked.
template<typename T>
bool WeekIntervalData<T>::insertRecord(Time timestamp, T &&record) {
    int idx = validate(timestamp, true);
    if (idx == -1) {
        return false;
    }
    data[idx] = record;
    return true;
}

// Populate the given record of a given timestmap, if not valid/available
// return 0.
template<typename T>
bool WeekIntervalData<T>::getRecord(Time timestamp, T &record) const {
    int idx = validate(timestamp, false);
    if (idx == -1 || !data[idx].has_value()) return false;
    record = data[idx].value();
    return true;
}

// Reset up all the data members.
template<typename T>
void WeekIntervalData<T>::clear() {
    week = 0;
    for (auto &ele: data) {
        ele.reset();
    }
}

// Helper function for insert and returns -1 if fails,
// returns appropriate index if valid.
// @param ignore_week : If week is not set yet in this object
// then, If ignore_week, set to the current timestamp else fail.
template<typename T>
int WeekIntervalData<T>::validate(Time timestamp, bool ignore_week) const {
    if (week != getWeek(timestamp) && week != 0) {
        std::cerr
                << "[WeekIntervalData] Interval usage, data loaded is for another week. Clear that first, and try re inserting"
                << std::endl;
        return -1;
    }

    // store week in object for later validation.
    if (week == 0) {
        if (!ignore_week) {
            return -1;
        } else {
            week = getWeek(timestamp);
        }
    }
    Day day = getDayOfTheWeek(timestamp);
    Minute minute = getMinuteOfTheDay(timestamp);

    if (day < START_DAY || day > END_DAY || minute < START_MINUTE || minute > END_MINUTE) {
        return -1;
    }
    return (END_MINUTE - START_MINUTE + 1) * day + (minute - START_MINUTE);
}

template<typename T>
void WeekIntervalData<T>::dump(const std::string &file_name) {
    std::ofstream outputFile(file_name, std::ios_base::app);

    if (outputFile.is_open()) {
        for (auto &obj: data) {
            if (obj.has_value()) {
                outputFile << obj.value().str() << std::endl;
            }
        }
        outputFile.close();
    } else {
        ERROR("Could not dump Pnl to " << file_name);
    }
}

#endif //CUBIST_WEEKINTERVALDATA_H
