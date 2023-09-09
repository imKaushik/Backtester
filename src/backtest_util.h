//
// Created by Kaushik on 04/09/23.
//

#ifndef CUBIST_BACKTEST_UTIL_H
#define CUBIST_BACKTEST_UTIL_H

/*
 * Has important constants, macros and helper functions.
 */

// You can control the log messages in the codes but turning on and off this.
#define LOGGING 0

// A simple macro for easy logging. Should ideally asynchronous Logging
// library like quill, but avoiding external libraries for now.
#if LOGGING == 1
#define LOG(message) \
    std::cout << message << std::endl;
#else
#define LOG(message)
#endif

// A simple macro for easy error logging. Should be printed at all time.
#define ERROR(message) \
    std::cerr << message << std::endl;

/*
 * Market hours : These control some of the internal
 * data structures. Be careful when changing them.
 */
const int START_MINUTE = 571; // 9:30 AM
const int END_MINUTE = 1021; // 5:00 PM, market hours till 4 but saw some entries till 5.
const int START_DAY = 0; // 0 - Monday
const int END_DAY = 4; // 4 - Friday
constexpr const int WEEK_TRADING_MINUTES = (END_MINUTE - START_MINUTE + 1) * (END_DAY - START_DAY + 1);

/*
 * Simple aliases so that reading code is easier. It's easier to read
 * from data type than variable name. The problem with this is "Type safety",
 * should look for an alternate which verifies type safety, and avoids
 * implicits conversion (Minutes to Day to Week, can be converted now).
 */
typedef long long Time;
typedef int Week;
typedef int Minute;
typedef int Day;
typedef int SecurityId;
typedef double Price;
typedef long double Quantity;
typedef long double PnL;

/*
 * Time utility functions
 */

// Note : In none of the operations we care about the DATE, so never worry about leap years and stuff.
const Time NANOSECOND_ADJ = 1000 * 1000 * 1000;
const Time NANOSECONDS_PER_MINUTE = 60 * NANOSECOND_ADJ;
const Time NANOSECONDS_PER_DAY = 24 * 60 * NANOSECONDS_PER_MINUTE;
const Time NANOSECONDS_PER_WEEK = 7 * NANOSECONDS_PER_DAY;
const Time EST_OFFSET = -4 * 60 * NANOSECONDS_PER_MINUTE;

// Get the epoch of the first second of the week in which the timestamp lies.
inline Week getWeek(Time timestamp) {
    // convert UTC to EST for accuracy
    timestamp += EST_OFFSET;
    timestamp -= 3 * NANOSECONDS_PER_DAY;
    return (Week)(timestamp/NANOSECONDS_PER_WEEK);
}

// Get Day of the week, 0 - Monday 6 - Sunday
inline Day getDayOfTheWeek(Time timestamp) {
    // convert UTC to EST for accuracy
    timestamp += EST_OFFSET;
    return ((Day)(timestamp / NANOSECONDS_PER_DAY) + 3) % 7;
}

// Get Minute of the day, 0 - 12:00 AM, 1439 - 11:59PM in EST
// Anything between X and X+1, should return X+1, assuming nothing
inline Minute getMinuteOfTheDay(Time timestamp) {
    // convert UTC to EST for accuracy
    timestamp += EST_OFFSET;
    Time seconds = ((timestamp % NANOSECONDS_PER_DAY));

    // The second part is to round off to the next number;
    return seconds / NANOSECONDS_PER_MINUTE + (seconds % NANOSECONDS_PER_MINUTE != 0);
}

// Get the first epoch of the week/day combination.
// Not timezone adjusted, gives as is. The user should
// adjust this as needed.
inline Time getFirstEpochOfDay(Week week, Day day)
{
    Time t = 0;
    t += (week+1) * NANOSECONDS_PER_WEEK;
    t -= (3-day) * NANOSECONDS_PER_DAY;
    return t;
}

// Round off to the next minute epoch.
// 02:10:12 to 02:11:00
// 02:10:00 will return as is.
inline Time roundOff(Time time)
{
    time /= NANOSECOND_ADJ;
    return ((time / 60) * 60 + (time % 60 != 0) * 60) * NANOSECOND_ADJ;
}

#endif //CUBIST_BACKTEST_UTIL_H
