
#include "VisibilityCalculator.h"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    constexpr double R = 6371.0;
    constexpr double ALTITUDE = 500.0;
    constexpr double EPSILON = 0.0001;

    // Objetivo terrestre en el ecuador, longitud 0°
    const GeoPosition target{R, 0.0, 0.0};

    // CASO 1: satélite directamente encima del objetivo
    const OrbitalPosition overhead{
        R + ALTITUDE, 0.0, 0.0
    };

    assert(VisibilityCalculator::isVisible(overhead, target));

    assert(std::abs(
        VisibilityCalculator::getElevationDegrees(
            overhead, target
        ) - 90.0
    ) < EPSILON);

    // CASO 2: satélite al otro lado de la Tierra
    const OrbitalPosition opposite{
        -(R + ALTITUDE), 0.0, 0.0
    };

    assert(!VisibilityCalculator::isVisible(opposite, target));

    // CASO 3: satélite exactamente sobre el horizonte
    // Su componente X debe ser igual al radio terrestre.
    const double tangentY = std::sqrt(
        (R + ALTITUDE) * (R + ALTITUDE) - R * R
    );

    const OrbitalPosition horizon{
        R, tangentY, 0.0
    };

    const double elevation =
        VisibilityCalculator::getElevationDegrees(
            horizon, target
        );

    assert(std::abs(elevation) < EPSILON);


    constexpr double CAMERA_FOV = 40.0;

    // CASO 1: satélite justo encima del objetivo
    assert(VisibilityCalculator::isTargetInCameraFov(
        overhead, target, CAMERA_FOV
    ));

    // CASO 2: satélite oculto por la Tierra
    assert(!VisibilityCalculator::isTargetInCameraFov(
        opposite, target, CAMERA_FOV
    ));

    // CASO 3: objetivo cercano, pero fuera del cono de 40°
    // Desplazamiento lateral de 500 km
    const OrbitalPosition offAxis{
        R + ALTITUDE, 500.0, 0.0
    };

    assert(VisibilityCalculator::isVisible(offAxis, target));

    assert(!VisibilityCalculator::isTargetInCameraFov(
        offAxis, target, CAMERA_FOV
    ));

    // CASO 4: pequeño desplazamiento lateral
    const OrbitalPosition nearAxis{
        R + ALTITUDE, 100.0, 0.0
    };

    assert(VisibilityCalculator::isTargetInCameraFov(
        nearAxis, target, CAMERA_FOV
    ));

    std::cout << "Camera FOV tests passed!\n";


    std::cout << "VisibilityCalculator tests passed!\n";
}
