
#include "GeoCoordinates.h"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    constexpr double EPSILON = 0.001;

    // Ecuador y meridiano de Greenwich
    const GeoPosition origin =
        GeoCoordinates::toCartesian(0.0, 0.0);

    assert(std::abs(origin.x - 6371.0) < EPSILON);
    assert(std::abs(origin.y) < EPSILON);
    assert(std::abs(origin.z) < EPSILON);

    // Polo norte
    const GeoPosition northPole =
        GeoCoordinates::toCartesian(90.0, 0.0);

    assert(std::abs(northPole.x) < EPSILON);
    assert(std::abs(northPole.y) < EPSILON);
    assert(std::abs(northPole.z - 6371.0) < EPSILON);

    // Madrid
    const GeoPosition madrid =
        GeoCoordinates::toCartesian(40.4168, -3.7038);

    std::cout << "Madrid:\n" << "X = " << madrid.x << " km\n" << "Y = " << madrid.y << " km\n" << "Z = " << madrid.z << " km\n";
    const GeoPosition rotated = GeoCoordinates::rotateWithEarth(madrid, 3600.0);

    const double originalRadius = std::sqrt(
        madrid.x * madrid.x +
        madrid.y * madrid.y +
        madrid.z * madrid.z
    );

    const double rotatedRadius = std::sqrt(
        rotated.x * rotated.x +
        rotated.y * rotated.y +
        rotated.z * rotated.z
    );

    // La rotación no cambia la distancia al centro terrestre
    assert(std::abs(originalRadius - rotatedRadius) < EPSILON);

    // Girar sobre Z no cambia la coordenada Z
    assert(std::abs(madrid.z - rotated.z) < EPSILON);

    std::cout << "\nMadrid despues de una hora:\n"
            << "X = " << rotated.x << " km\n"
            << "Y = " << rotated.y << " km\n"
            << "Z = " << rotated.z << " km\n";


    std::cout << "GeoCoordinates tests passed!\n";
}
