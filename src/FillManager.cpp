//
// Created by Kaushik on 08/09/23.
//
#include "FillManager.h"

// Push a new fill record.
bool FillManager::addFill(Fill &&record) {
    return SecurityUniverse::getInstance().addFill(std::move(record));
}

// Push a new fill record.
bool FillManager::addFill(Fill &record) {
    return SecurityUniverse::getInstance().addFill(record);
}

// Clean up the fill records.
void FillManager::postProcess() {
    SecurityUniverse::getInstance().postProcessFill();
}