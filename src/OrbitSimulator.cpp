#include <iostream>
#include <cmath>
#include "OrbitSimulator.h"

namespace {
    constexpr double PI = 3.14159265358979323846;
}

double OrbitSimulator::getOrbitalRadiusKm() const{
    return earthRadiusKm + altitudeKm;
}


double OrbitSimulator::getOrbitalSpeedKmS() const{
    return std::sqrt(mu / getOrbitalRadiusKm());
}


double OrbitSimulator::getOrbitalPeriodSeconds() const{
    return (2*PI*getOrbitalRadiusKm())/getOrbitalSpeedKmS();
}

OrbitalPosition OrbitSimulator::getPosition(double timeSeconds) const{
    double angulo = (2 * PI * timeSeconds) / getOrbitalPeriodSeconds();
    double r = getOrbitalRadiusKm();

    return {
        r * std::cos(angulo),
        r * std::sin(angulo),
        0.0
    };
}