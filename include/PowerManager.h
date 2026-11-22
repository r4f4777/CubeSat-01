
#pragma once

#include <string>

enum class SatelliteMode {
    NORMAL,
    LOW_POWER,
    SAFE
};

class PowerManager {
private:
    SatelliteMode currentMode = SatelliteMode::NORMAL;

public:
    void update(float batteryLevel);

    SatelliteMode getMode() const;

    std::string getModeString() const;

    void enterSafeMode();
};
