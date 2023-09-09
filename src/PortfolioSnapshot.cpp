//
// Created by Kaushik on 08/09/23.
//
#include "PortfolioSnapshot.h"

// Get a string representation
std::string PortfolioSnapshot::str() const
{
    return "{Timestamp: " + std::to_string(timestamp) + ", "
           +"cash: " + std::to_string(cash) + ", "
           +"position: " + std::to_string(position) + "}";
}

// Reset the data members.
void PortfolioSnapshot::clear()
{
    cash = 0;
    position = 0;
    timestamp = 0;
}

// Get the associated timestamp
Time PortfolioSnapshot::getTime() const
{
    return timestamp;
}

// Not implemented
bool PortfolioSnapshot::parseCSVRow(char line[1024])
{
    throw std::runtime_error("PortfolioSnapshot::parseCSVRow is not implemented");
    return false;
}
