#pragma once

#include "OrbitSimulator.h"
#include "GeoCoordinates.h"

class VisibilityCalculator {
public:
    static double getElevationDegrees(
        const OrbitalPosition& satellite,
        const GeoPosition& target
    );

    static bool isVisible(
        const OrbitalPosition& satellite,
        const GeoPosition& target
    );

    static bool isTargetInCameraFov(
        const OrbitalPosition& satellite,
        const GeoPosition& target,
        double fovDegrees
    );
};
