
#include "VisibilityCalculator.h"

#include <cmath>
#include <stdexcept>

namespace {
    constexpr double PI = 3.14159265358979323846;
}

double VisibilityCalculator::getElevationDegrees(
    const OrbitalPosition& satellite,
    const GeoPosition& target
) {
    // Vector desde Madrid hacia el satélite
    const double dx = satellite.x - target.x;
    const double dy = satellite.y - target.y;
    const double dz = satellite.z - target.z;

    const double distance = std::sqrt(
        dx * dx + dy * dy + dz * dz
    );

    const double targetRadius = std::sqrt(
        target.x * target.x +
        target.y * target.y +
        target.z * target.z
    );

    if (distance == 0.0 || targetRadius == 0.0) {
        throw std::invalid_argument("Invalid position");
    }

    // Producto escalar con la vertical local de Madrid
    const double dot =
        dx * target.x +
        dy * target.y +
        dz * target.z;

    const double sinElevation =
        dot / (distance * targetRadius);

    // Evitar errores de redondeo fuera de [-1, 1]
    const double safeValue = std::fmax(
        -1.0,
        std::fmin(1.0, sinElevation)
    );

    return std::asin(safeValue) * 180.0 / PI;
}

bool VisibilityCalculator::isVisible(
    const OrbitalPosition& satellite,
    const GeoPosition& target
) {
    return getElevationDegrees(satellite, target) > 0.0;
}


bool VisibilityCalculator::isTargetInCameraFov(
    const OrbitalPosition& satellite,
    const GeoPosition& target,
    double fovDegrees
) {
    if (!std::isfinite(fovDegrees) ||
        fovDegrees <= 0.0 ||
        fovDegrees > 180.0) {
        throw std::invalid_argument("Invalid camera FOV");
    }

    // Primero: Madrid debe estar sobre el horizonte
    if (!isVisible(satellite, target)) {
        return false;
    }

    // Vector del satélite hacia el centro de la Tierra (nadir)
    const double nx = -satellite.x;
    const double ny = -satellite.y;
    const double nz = -satellite.z;

    // Vector del satélite hacia Madrid
    const double tx = target.x - satellite.x;
    const double ty = target.y - satellite.y;
    const double tz = target.z - satellite.z;

    // Producto escalar
    const double dot = nx * tx + ny * ty + nz * tz;

    const double nadirLength = std::sqrt(
        nx * nx + ny * ny + nz * nz
    );

    const double targetLength = std::sqrt(
        tx * tx + ty * ty + tz * tz
    );

    if (nadirLength == 0.0 || targetLength == 0.0) {
        throw std::invalid_argument("Invalid satellite position");
    }

    // Coseno del ángulo entre ambas direcciones
    const double cosAngle =
        dot / (nadirLength * targetLength);

    const double safeCos = std::fmax(
        -1.0, std::fmin(1.0, cosAngle)
    );

    const double angleDegrees =
        std::acos(safeCos) * 180.0 / PI;

    return angleDegrees <= fovDegrees / 2.0;
}
