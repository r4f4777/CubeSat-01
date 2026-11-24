#include "SatelliteController.h"

#include <iostream>
#include <cmath>
#include "OrbitSimulator.h"
#include "GeoCoordinates.h"
#include "VisibilityCalculator.h"

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

bool SatelliteController::requestPhoto() {

    // 1. Consultar el modo energético.
    if(power.getMode() != SatelliteMode::NORMAL){
        return false;
    }
    else{
        return camera.takePhoto();
    }
}


bool SatelliteController::requestPhotoOfTarget(
    const OrbitalPosition& satellite,
    const GeoPosition& target,
    double cameraFovDegrees
) {
    // 1. ¿Está el objetivo dentro del campo de visión?
    const bool canObserve =
        VisibilityCalculator::isTargetInCameraFov(
            satellite,
            target,
            cameraFovDegrees
        );

    if (!canObserve) {
        return false;
    }

    // 2. Reutilizar las comprobaciones existentes:
    // batería, modo energético y disponibilidad de cámara.
    return requestPhoto();
}


int SatelliteController::getPhotosTaken() const {
    return camera.getPhotosTaken();
}

bool SatelliteController::schedulePhoto(const PhotoCommand& command) {
    return mission.schedulePhoto(command);
}

void SatelliteController::processMissionAtTime(int currentTime) {

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

    // Las misiones para Madrid necesitan visibilidad orbital.
    if (mission.isPendingTarget("Madrid")) {

        // Órbita inclinada 51,6 grados
        const OrbitSimulator orbit(51.6);

        // Posición del CubeSat en el instante actual
        const OrbitalPosition satellitePosition =
            orbit.getPosition(currentTime);

        // Posición inicial de Madrid
        const GeoPosition madrid =
            GeoCoordinates::toCartesian(40.4168, -3.7038);

        // Madrid gira con la Tierra
        const GeoPosition madridPosition =
            GeoCoordinates::rotateWithEarth(
                madrid,
                currentTime
            );

        // Cámara orientada al nadir, FOV total de 40 grados
        const bool canPhotograph =
            VisibilityCalculator::isTargetInCameraFov(
                satellitePosition,
                madridPosition,
                40.0
            );

        // Sin visibilidad, la misión permanece PENDING.
        if (!canPhotograph) {
            return;
        }
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


void SatelliteController::updateMission(int currentTime) {

    if (currentTime < 0) {
        return;
    }

    // Primera actualización: comprobar el instante actual.
    if (lastMissionUpdateTime == -1) {
        lastMissionUpdateTime = currentTime;
        processMissionAtTime(currentTime);
        return;
    }

    // Evitar volver a procesar tiempos anteriores.
    if (currentTime <= lastMissionUpdateTime) {
        return;
    }

    // Comprobar cada segundo simulado intermedio.
    for (int t = lastMissionUpdateTime + 1;
         t <= currentTime;
         ++t) {

        processMissionAtTime(t);
    }

    lastMissionUpdateTime = currentTime;
}


bool SatelliteController::recoverMission() {
    return mission.loadCommand();
}

bool SatelliteController::missionNeedsReview() const {
    return mission.needsReview();
}

void SatelliteController::tick(){
    clock.tick();
    updateMission(clock.getTime());
}

int SatelliteController::getMissionTime() const{
    return clock.getTime();
}

bool SatelliteController::setSimulationSpeed(int speed){
    return clock.setSpeed(speed);
}

CommandState SatelliteController::getMissionState() const {
    return mission.getCommandState();
}