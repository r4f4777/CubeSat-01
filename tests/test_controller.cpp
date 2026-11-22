
#include <cassert>
#include <iostream>
#include "SatelliteController.h"

int main() {
    SatelliteController satellite("controller_test.dat");

    assert(satellite.schedulePhoto({"MADRID", 30, 75}));

    // Antes de la ventana de fotografía
    satellite.processBatteryReading(85);
    satellite.updateMission(20);
    assert(satellite.getPhotosTaken() == 0);

    // Dentro de la ventana, pero con batería baja
    satellite.processBatteryReading(20);
    satellite.updateMission(40);
    assert(satellite.getModeString() == "LOW_POWER");
    assert(satellite.getPhotosTaken() == 0);

    // Batería recuperada: debe fotografiar
    satellite.processBatteryReading(45);
    satellite.updateMission(60);
    assert(satellite.getModeString() == "NORMAL");
    assert(satellite.getPhotosTaken() == 1);

    // No debe repetir la fotografía
    satellite.updateMission(65);
    assert(satellite.getPhotosTaken() == 1);

    std::cout << "SatelliteController tests: PASSED\n";

    return 0;
}
