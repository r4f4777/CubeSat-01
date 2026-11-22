#pragma once

#include <deque>
#include <PowerManager.h>
#include <SensorValidator.h>
#include <CameraManager.h>
#include <MissionManager.h>
#include "MissionClock.h"

class SatelliteController {

private:
    PowerManager power;
    SensorValidator validator;
    CameraManager camera;
    MissionManager mission;
    MissionClock clock;

public:

    explicit SatelliteController(
        const std::string& filename = "mission.dat"
    );

   void processBatteryReading(float batteryLevel);

    std::string getModeString() const;

    bool requestPhoto();

    int getPhotosTaken() const;

    bool schedulePhoto(const PhotoCommand& command);

    void updateMission(int currentTime);

    bool recoverMission();

    bool missionNeedsReview() const;

    void tick();

    int getMissionTime() const;
    
    bool setSimulationSpeed(int speed);

};