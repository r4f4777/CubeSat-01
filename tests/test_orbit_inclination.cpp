
#include "OrbitSimulator.h"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    constexpr double EPSILON = 0.001;
    constexpr double PI = 3.14159265358979323846;

    OrbitSimulator equatorial;
    OrbitSimulator inclined(51.6);

    const double quarterPeriod =
        inclined.getOrbitalPeriodSeconds() / 4.0;

    // La órbita ecuatorial mantiene Z = 0
    const auto oldPosition =
        equatorial.getPosition(quarterPeriod);

    assert(std::abs(oldPosition.z) < EPSILON);

    // A un cuarto de órbita alcanzamos la máxima latitud
    const auto position =
        inclined.getPosition(quarterPeriod);

    const double expectedZ =
        inclined.getOrbitalRadiusKm() *
        std::sin(51.6 * PI / 180.0);

    assert(std::abs(position.z - expectedZ) < EPSILON);

    // La distancia al centro terrestre debe ser constante
    const double distance = std::sqrt(
        position.x * position.x +
        position.y * position.y +
        position.z * position.z
    );

    assert(
        std::abs(distance - inclined.getOrbitalRadiusKm())
        < EPSILON
    );

    std::cout << "Orbital inclination tests passed!\n";
}
