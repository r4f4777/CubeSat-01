#include "PowerManager.h"

void PowerManager::update(float batteryLevel) {
    switch (currentMode) {
        case SatelliteMode::NORMAL:
            if(batteryLevel < 15){
                currentMode = SatelliteMode::SAFE;
            }  
            else if(batteryLevel < 30){
                currentMode = SatelliteMode::LOW_POWER;
            }
        break;

        case SatelliteMode::LOW_POWER:
            if(batteryLevel > 40){
                currentMode = SatelliteMode::NORMAL;
            }
            if(batteryLevel < 15){
                currentMode = SatelliteMode::SAFE;
            }
        break;

        case SatelliteMode::SAFE:
            if(batteryLevel > 25){
                currentMode = SatelliteMode::LOW_POWER;
            }
        break;
    }
}

SatelliteMode PowerManager::getMode() const {
    return currentMode;
}

std::string PowerManager::getModeString() const {
    switch (getMode()) {
        case SatelliteMode::NORMAL:    return "NORMAL";
        case SatelliteMode::LOW_POWER: return "LOW_POWER";
        case SatelliteMode::SAFE:      return "SAFE";
        default:                       return "UNKNOWN";
    }
}

void PowerManager::enterSafeMode() {
    currentMode = SatelliteMode::SAFE;
}