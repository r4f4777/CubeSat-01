
#include <cassert>
#include <iostream>
#include "SatelliteController.h"

int main() {

    // ==========================================
    // TEST 1: MISION CON CAMBIOS DE BATERIA
    // ==========================================
    {
        SatelliteController satellite("controller_test.dat");

        assert(satellite.schedulePhoto({"MADRID", 30, 75}));

        // Antes de la ventana
        satellite.processBatteryReading(85);
        satellite.updateMission(20);
        assert(satellite.getPhotosTaken() == 0);

        // Dentro de la ventana, bateria baja
        satellite.processBatteryReading(20);
        satellite.updateMission(40);
        assert(satellite.getModeString() == "LOW_POWER");
        assert(satellite.getPhotosTaken() == 0);

        // Recuperacion de bateria
        satellite.processBatteryReading(45);
        satellite.updateMission(60);
        assert(satellite.getModeString() == "NORMAL");
        assert(satellite.getPhotosTaken() == 1);

        // No repetir fotografia
        satellite.updateMission(65);
        assert(satellite.getPhotosTaken() == 1);
    }

    // ==========================================
    // TEST 2: RELOJ DE MISION
    // ==========================================
    {
        SatelliteController satellite("clock_controller_test.dat");

        assert(satellite.getMissionTime() == 0);

        satellite.tick();
        assert(satellite.getMissionTime() == 1);

        assert(satellite.setSimulationSpeed(10));

        satellite.tick();
        assert(satellite.getMissionTime() == 11);

        // Velocidad invalida
        assert(!satellite.setSimulationSpeed(50));

        satellite.tick();
        assert(satellite.getMissionTime() == 21);
    }

    // ==========================================
    // TEST 3: FOTOGRAFIA AUTOMATICA
    // ==========================================
    {
        SatelliteController satellite("clock_integration_test.dat");

        assert(satellite.schedulePhoto({"MADRID", 30, 75}));
        assert(satellite.setSimulationSpeed(10));

        // Segundo 20: todavia no fotografiar
        satellite.tick();
        satellite.tick();

        assert(satellite.getMissionTime() == 20);
        assert(satellite.getPhotosTaken() == 0);

        // Segundo 40: ejecutar fotografia
        satellite.tick();
        satellite.tick();

        assert(satellite.getMissionTime() == 40);
        assert(satellite.getPhotosTaken() == 1);

        // Segundo 80: no repetir fotografia
        for (int i = 0; i < 4; i++) {
            satellite.tick();
        }

        assert(satellite.getMissionTime() == 80);
        assert(satellite.getPhotosTaken() == 1);
    }

    // ==========================================
    // TEST 4: FOTOGRAFIA CON BATERIA BAJA
    // ==========================================
    {
        SatelliteController satellite("clock_low_power_test.dat");

        assert(satellite.schedulePhoto({"MADRID", 30, 75}));

        // Bateria baja
        satellite.processBatteryReading(20);
        assert(satellite.getModeString() == "LOW_POWER");

        assert(satellite.setSimulationSpeed(10));

        // Avanzar hasta segundo 40
        for (int i = 0; i < 4; i++) {
            satellite.tick();
        }

        assert(satellite.getMissionTime() == 40);
        assert(satellite.getPhotosTaken() == 0);

        // Recuperar bateria
        satellite.processBatteryReading(50);
        assert(satellite.getModeString() == "NORMAL");

        satellite.tick();

        // Segundo 50: ejecutar fotografia
        assert(satellite.getMissionTime() == 50);
        assert(satellite.getPhotosTaken() == 1);
    }

    std::cout << "SatelliteController tests: PASSED\n";

    return 0;
}
