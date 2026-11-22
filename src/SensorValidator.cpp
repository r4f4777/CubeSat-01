#include "SensorValidator.h"

#include <iostream>
#include <cmath>


bool SensorValidator::isValidBattery(float batteryLevel) {
    bool valid = std::isfinite(batteryLevel) && batteryLevel >= 0.0f && batteryLevel <= 100.0f;
    if (valid) {
        return true;
    }

    std::cout << "ERROR: Invalid battery " << batteryLevel << "%" << std::endl;
    return false;
}

bool SensorValidator::processReading(float batteryLevel) {
    bool valid = isValidBattery(batteryLevel);

    if (valid) {
        consecutiveErrors = 0;
        consecutiveValid++;
    } else {
        consecutiveErrors++;
        consecutiveValid = 0;
    }

    recentReadings.push_back(valid);

    if (recentReadings.size() > 10) {
        recentReadings.pop_front();
    }

    if (consecutiveErrors >= 3 || hasIntermittentFault()) {
        sensorFault = true;
    }

    if (sensorFault && consecutiveValid >= 5 && !hasIntermittentFault()) {
        sensorFault = false;
    }

    return valid;
}

bool SensorValidator::hasIntermittentFault() const {
    int errors = 0;

    for (bool valid : recentReadings) {
        if (!valid) {
            errors++;
        }
    }

    return errors >= 3;
}

bool SensorValidator::hasFault() const {
    return sensorFault;
}

int SensorValidator::getConsecutiveErrors() const {
    return consecutiveErrors;
}