#pragma once

#include <deque>


enum class CameraState {
    IDLE,
    CAPTURING
};

class CameraManager {
private:
    CameraState state = CameraState::IDLE;
    int photosTaken = 0;
public:
    bool takePhoto();

    int getPhotosTaken() const;

};