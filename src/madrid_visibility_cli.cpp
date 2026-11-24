
#include "OrbitSimulator.h"
#include "GeoCoordinates.h"
#include "VisibilityCalculator.h"

#include <iostream>
#include <iomanip>

int main() {
    OrbitSimulator satellite(51.6);

    const GeoPosition madrid =
        GeoCoordinates::toCartesian(40.4168, -3.7038);

    constexpr int STEP_SECONDS = 10;
    constexpr int DURATION_SECONDS = 48 * 3600;

    bool wasVisible = false;
    bool wasInCameraFov = false;
    int photoWindowStart = 0;
    int windowStart = 0;
    double maxElevation = 0.0;

    for (int t = 0; t <= DURATION_SECONDS; t += STEP_SECONDS) {

        const OrbitalPosition satellitePosition =
            satellite.getPosition(t);

        const GeoPosition madridPosition =
            GeoCoordinates::rotateWithEarth(madrid, t);

        const double elevation =
            VisibilityCalculator::getElevationDegrees(
                satellitePosition,
                madridPosition
            );

        const bool visible = elevation > 0.0;

        const bool inCameraFov =
            VisibilityCalculator::isTargetInCameraFov(
                satellitePosition,
                madridPosition,
                40.0
            );

        // Madrid acaba de entrar en el campo de visión
        if (inCameraFov && !wasInCameraFov) {
            photoWindowStart = t;

            std::cout << "\nPHOTO WINDOW START: "
                    << t << " s\n";
        }

        // Madrid acaba de salir del campo de visión
        if (!inCameraFov && wasInCameraFov) {
            std::cout << "PHOTO WINDOW END: "
                    << t << " s\n";

            std::cout << "Photo opportunity duration: "
                    << t - photoWindowStart
                    << " s\n";

            std::cout << "=======================\n";
        }

        wasInCameraFov = inCameraFov;


        if (visible && !wasVisible) {
            windowStart = t;
            maxElevation = elevation;

            std::cout << "Madrid VISIBLE desde t="
                      << t << " s\n";
        }

        if (visible && elevation > maxElevation) {
            maxElevation = elevation;
        }

        if (!visible && wasVisible) {
            std::cout << "Madrid deja de ser visible en t="
                      << t << " s\n";

            std::cout << "Duracion aproximada: "
                      << t - windowStart << " s\n";

            std::cout << std::fixed
                      << std::setprecision(2)
                      << "Elevacion maxima: "
                      << maxElevation << " grados\n";

            std::cout << "-----------------------\n";
        }

        wasVisible = visible;
    }

    if (wasVisible) {
        std::cout << "Ventana aun abierta al finalizar el analisis.\n";
    }

    return 0;
}
