#pragma once

#include <deque>

class SensorValidator {

private:
    int consecutiveErrors = 0;
    int consecutiveValid = 0;
    bool sensorFault = false;
    std::deque<bool> recentReadings;

public:
    bool isValidBattery(float batteryLevel);

    bool processReading(float batteryLevel);

    bool hasIntermittentFault() const;

    bool hasFault() const;

    int getConsecutiveErrors() const;
};