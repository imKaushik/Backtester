//
// Created by Kaushik on 07/09/23.
//
#include "MarketDataManager.h"

/*
 * The file name is necessary to create the market data reader.
 * Passes it forward to SecurityUniverse to create and manage the
 * Security Universe
 */
MarketDataManager::MarketDataManager(const std::string &file_name)
{
    SecurityUniverse::getInstance().setDataSource(file_name);
}

// Load the market data related to this week into memory.
void MarketDataManager::loadMarketData(Week week) noexcept(false)
{
    SecurityUniverse::getInstance().loadMarketData(week);
}

// Fetch the market data informaton for a security at a given time.
bool MarketDataManager::getMarketData(SecurityId security_id, Time time, MinuteBarRecord& record) const
{
    return SecurityUniverse::getInstance().getMarketData(security_id, time, record);
}

// Clean up the market data created for this week.
void MarketDataManager::postProcess()
{
    return SecurityUniverse::getInstance().postProcessMarketData();
}