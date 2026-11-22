#include "SatelliteController.h"

#include <iostream>
#include <cmath>

SatelliteController::SatelliteController(
    const std::string& filename
) : mission(filename) {
}

void SatelliteController::processBatteryReading(float batteryLevel) {
    
        bool valid = validator.processReading(batteryLevel);

        if(validator.hasFault()){
            power.enterSafeMode();
        }
        else{
            if(valid){
                power.update(batteryLevel);
            }
        }
    }

std::string SatelliteController::getModeString() const {
    return power.getModeString();
}

bool SatelliteController::SatelliteController::requestPhoto() {

    // 1. Consultar el modo energético.
    if(power.getMode() != SatelliteMode::NORMAL){
        return false;
    }
    else{
        return camera.takePhoto();
    }
}

int SatelliteController::getPhotosTaken() const {
    return camera.getPhotosTaken();
}

bool SatelliteController::schedulePhoto(const PhotoCommand& command) {
    return mission.schedulePhoto(command);
}

void SatelliteController::updateMission(int currentTime) {

    if (mission.hasExpired(currentTime)) {
        mission.expireCommand();
        return;
    }
    
    if(!mission.isPhotoWindowOpen(currentTime)){
        return;
    }

    // No iniciamos la cámara si la batería
    // no permite fotografiar.
    if (power.getMode() != SatelliteMode::NORMAL) {
        return;
    }

    // Registrar EXECUTING antes de fotografiar.
    if (!mission.beginExecution()) {
        return;
    }

    bool success = requestPhoto();

    if (success) {
        mission.completeCommand();
    }

}

bool SatelliteController::recoverMission() {
    return mission.loadCommand();
}

bool SatelliteController::missionNeedsReview() const {
    return mission.needsReview();
}
