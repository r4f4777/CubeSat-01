
#include <cassert>
#include <iostream>
#include "PowerManager.h"

int main() {
    PowerManager power;

    // Batería inicial: modo NORMAL
    assert(power.getMode() == SatelliteMode::NORMAL);

    // Batería al 20%: LOW_POWER
    power.update(20);
    assert(power.getMode() == SatelliteMode::LOW_POWER);

    // Batería al 10%: SAFE
    power.update(10);
    assert(power.getMode() == SatelliteMode::SAFE);

    // Con 26%, SAFE debe pasar a LOW_POWER
    power.update(26);
    assert(power.getMode() == SatelliteMode::LOW_POWER);

    // Con 35%, debe seguir en LOW_POWER
    power.update(35);
    assert(power.getMode() == SatelliteMode::LOW_POWER);

    // Con 45%, debe recuperar NORMAL
    power.update(45);
    assert(power.getMode() == SatelliteMode::NORMAL);

    std::cout << "PowerManager tests: PASSED\n";

    return 0;
}
