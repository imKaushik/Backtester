//
// Created by Kaushik on 08/09/23.
//
#include "Fill.h"

// Fetch a string representation
std::string Fill::str() const {
    return "{SecurityId: " + std::to_string(security_id) + ", "
           + "Fill time: " + std::to_string(fill_time) + ", "
           + "Execute time: " + std::to_string(execute_time) + ", "
           + "Fill Quantity: " + std::to_string(fill_quantity) + ", "
           + "Fill Price: " + std::to_string(fill_price) + "}";
}

// Reset the data members.
void Fill::clear() {
    security_id = 0;
    fill_time = 0;
    execute_time = 0;
    fill_price = 0;
    fill_quantity = 0;
}

// Get the associated timestamp.
Time Fill::getTime() const {
    return execute_time;
}

// Populate data members after parsing a CSV row - Not implemented
bool Fill::parseCSVRow(char line[1024]) {
    throw std::runtime_error("Fill::parseCSVRow is not implemented.");
    return false;
}