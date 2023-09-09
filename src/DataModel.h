//
// Created by Kaushik on 08/09/23.
//

#ifndef CUBIST_DATAMODEL_H
#define CUBIST_DATAMODEL_H
#include <string>
#include <iostream>
#include "backtest_util.h"

/*
 * Creating a basic data model that every data object, that
 * we need to store for time-series has to inherit from.
 * It's primarily to impose the "time" component, along
 * with some basic functionality for templates to work
 * with classes inherited from Data Model.
 */
class DataModel
{
public:
    DataModel() = default;

    // Get string representation
    virtual std::string str() const = 0;

    // Reset all data members.
    virtual void clear() = 0;

    // Get the timestamp associated with this object.
    virtual Time getTime() const = 0;

    // Populate data members after parsing a CSV row
    virtual bool parseCSVRow(char line[1024]) = 0;
};

/* Helper Template
 */
// Define a type trait to check if a class derives from VirtualBase
template <typename T>
struct is_derived_from_data_model {
    static constexpr bool value = std::is_base_of<DataModel, T>::value;
};

#endif //CUBIST_DATAMODEL_H
