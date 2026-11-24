
#include "GeoCoordinates.h"

#include <cmath>

GeoPosition GeoCoordinates::toCartesian(
    double latitudeDegrees,
    double longitudeDegrees,
    double altitudeKm
) {
    constexpr double EARTH_RADIUS_KM = 6371.0;
    constexpr double PI = 3.14159265358979323846;

    const double latitude =
        latitudeDegrees * PI / 180.0;

    const double longitude =
        longitudeDegrees * PI / 180.0;

    const double radius = EARTH_RADIUS_KM + altitudeKm;

    return {
        radius * std::cos(latitude) * std::cos(longitude),
        radius * std::cos(latitude) * std::sin(longitude),
        radius * std::sin(latitude)
    };
}

GeoPosition GeoCoordinates::rotateWithEarth(
    const GeoPosition& position,
    double elapsedSeconds
) {
    // Velocidad angular terrestre aproximada (rad/s)
    constexpr double EARTH_ROTATION_RATE = 7.2921159e-5;

    const double angle =
        EARTH_ROTATION_RATE * elapsedSeconds;

    const double cosAngle = std::cos(angle);
    const double sinAngle = std::sin(angle);

    return {
        position.x * cosAngle - position.y * sinAngle,
        position.x * sinAngle + position.y * cosAngle,
        position.z
    };
}