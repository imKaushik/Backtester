//
// Created by Kaushik on 08/09/23.
//
#include "PnLRecord.h"

// Get a string representation.
std::string PnLRecord::str() const
{
    return "{Timestamp: " + std::to_string(timestamp) +
           ", Pnl: " + std::to_string(pnl) + "}";
}

// Reset the data members.
void PnLRecord::clear()
{
    timestamp = 0;
    pnl = 0;
}

// Get the associated timestamp.
Time PnLRecord::getTime() const
{
    return timestamp;
}

// Not implemented
bool PnLRecord::parseCSVRow(char line[1024])
{
    throw std::runtime_error("PnLRecord::parseCSVRow is not implemented.");
    return false;
}