#pragma once

struct GeoPosition {
    double x;
    double y;
    double z;
};

class GeoCoordinates {
public:
    static GeoPosition toCartesian(
        double latitudeDegrees,
        double longitudeDegrees,
        double altitudeKm = 0.0
    );

    static GeoPosition rotateWithEarth(
        const GeoPosition& position,
        double elapsedSeconds
    );
};
