
#include "SatelliteController.h"
#include "VisibilityCalculator.h"

#include <cassert>
#include <iostream>

int main() {
    constexpr double R = 6371.0;

    // Usamos un archivo de misión propio para esta prueba.
    SatelliteController satellite("test_photo_visibility.dat");

    const GeoPosition target{R, 0.0, 0.0};

    // Madrid simulado justo debajo del satélite.
    const OrbitalPosition overhead{R + 500.0, 0.0, 0.0};

    // Objetivo fuera del campo de visión.
    const OrbitalPosition offAxis{R + 500.0, 500.0, 0.0};

    const int initialPhotos = satellite.getPhotosTaken();

    // Fuera de cámara: NO debe fotografiar.
    assert(!satellite.requestPhotoOfTarget(offAxis, target));

    assert(satellite.getPhotosTaken() == initialPhotos);

    std::cout << "Out-of-FOV photo correctly rejected!\n";

    // Con el objetivo dentro del campo de visión,
    // la autorización final depende de batería y cámara.
    const bool result =
        satellite.requestPhotoOfTarget(overhead, target);

    std::cout << "Photo request inside FOV: "
              << (result ? "ACCEPTED" : "REJECTED")
              << "\n";

    // ==========================================
    // MISION AUTOMATICA: FOTOGRAFIAR MADRID
    // ==========================================
    {
        SatelliteController controller("test_madrid_auto.dat");

        PhotoCommand command{"Madrid", 97200, 97300};

        assert(controller.schedulePhoto(command));
        assert(controller.getMissionState() == CommandState::PENDING);

        // 1. Madrid todavía está fuera del campo de visión
        controller.updateMission(97200);

        assert(controller.getPhotosTaken() == 0);
        assert(controller.getMissionState() == CommandState::PENDING);

        std::cout << "t=97200: Waiting for Madrid...\n";

        // 2. Madrid entra en el campo de visión
        controller.updateMission(97230);

        assert(controller.getPhotosTaken() == 1);
        assert(controller.getMissionState() == CommandState::COMPLETED);

        std::cout << "t=97230: Madrid photographed!\n";

        // 3. No debe repetirse la fotografía
        controller.updateMission(97240);

        assert(controller.getPhotosTaken() == 1);
        assert(controller.getMissionState() == CommandState::COMPLETED);

        std::cout << "No duplicate photos!\n";
    }


    {
        SatelliteController controller("test_madrid_fast.dat");

        PhotoCommand command{"Madrid", 97200, 97300};
        assert(controller.schedulePhoto(command));

        // Simulamos un reloj que salta toda la ventana fotográfica.
        controller.updateMission(97200);
        controller.updateMission(97300);

        // Idealmente debería haberse detectado la oportunidad.
        assert(controller.getPhotosTaken() == 1);
        assert(controller.getMissionState() == CommandState::COMPLETED);

        std::cout << "Fast simulation window detected!\n";
    }


    std::cout << "Photo visibility integration test passed!\n";

    return 0;


}
