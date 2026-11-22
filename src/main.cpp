#include <iostream>
#include "SatelliteController.h"

int main() {
    SatelliteController satellite;

    if (!satellite.schedulePhoto({"MADRID", 30, 75})) {
        std::cout << "ERROR: Command not saved\n";
        return 1;
    }

    int times[] = {20, 40, 60, 65};
    float battery[] = {85, 20, 45, 45};

    for (int i = 0; i < 4; i++) {
        satellite.processBatteryReading(battery[i]);
        satellite.updateMission(times[i]);

        std::cout << "Time: " << times[i]
                  << "s | Mode: " << satellite.getModeString()
                  << " | Photos: " << satellite.getPhotosTaken()
                  << '\n';
    }

    return 0;
}
