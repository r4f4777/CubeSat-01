#include <cassert>
#include <cmath>
#include <iostream>
#include "OrbitSimulator.h"

int main() {
    OrbitSimulator orbit;

    // Radio orbital: 6371 + 500 = 6871 km
    assert(std::abs(orbit.getOrbitalRadiusKm() - 6871.0) < 0.001);

    // Velocidad orbital aproximada: 7.62 km/s
    assert(std::abs(orbit.getOrbitalSpeedKmS() - 7.62) < 0.01);

    // Periodo orbital aproximado: 5668 segundos
    assert(std::abs(orbit.getOrbitalPeriodSeconds() - 5668.0) < 10.0);


    OrbitalPosition start = orbit.getPosition(0.0);

    assert(std::abs(start.x - 6871.0) < 0.001);
    assert(std::abs(start.y) < 0.001);
    assert(std::abs(start.z) < 0.001);

    OrbitalPosition quarter = orbit.getPosition(
        orbit.getOrbitalPeriodSeconds() / 4.0
    );

    assert(std::abs(quarter.x) < 0.001);
    assert(std::abs(quarter.y - 6871.0) < 0.001);

    std::cout << "OrbitSimulator tests: PASSED\n";
    return 0;
}
