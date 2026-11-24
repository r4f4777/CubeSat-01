
#include <cmath>
#include "OrbitSimulator.h"

namespace {
    constexpr double PI = 3.14159265358979323846;
}

OrbitSimulator::OrbitSimulator(double inclinationDegrees)
    : inclinationDegrees(inclinationDegrees) {}

double OrbitSimulator::getOrbitalRadiusKm() const {
    return earthRadiusKm + altitudeKm;
}

double OrbitSimulator::getOrbitalSpeedKmS() const {
    return std::sqrt(mu / getOrbitalRadiusKm());
}

double OrbitSimulator::getOrbitalPeriodSeconds() const {
    return (2 * PI * getOrbitalRadiusKm()) / getOrbitalSpeedKmS();
}

OrbitalPosition OrbitSimulator::getPosition(double timeSeconds) const {
    const double angle =
        (2 * PI * timeSeconds) / getOrbitalPeriodSeconds();

    const double inclination =
        inclinationDegrees * PI / 180.0;

    const double r = getOrbitalRadiusKm();

    return {
        r * std::cos(angle),
        r * std::sin(angle) * std::cos(inclination),
        r * std::sin(angle) * std::sin(inclination)
    };
}
