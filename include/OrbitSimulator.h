
#pragma once

struct OrbitalPosition {
    double x;
    double y;
    double z;
};

class OrbitSimulator {
private:
    double altitudeKm = 500.0;
    double earthRadiusKm = 6371.0;
    double mu = 398600.4418;
    double inclinationDegrees = 0.0;

public:
    explicit OrbitSimulator(double inclinationDegrees = 0.0);

    double getOrbitalRadiusKm() const;
    double getOrbitalSpeedKmS() const;
    double getOrbitalPeriodSeconds() const;
    OrbitalPosition getPosition(double timeSeconds) const;
};
