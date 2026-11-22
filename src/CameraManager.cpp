#include "CameraManager.h"

#include <iostream>
#include <cmath>

bool CameraManager::takePhoto() {
        if(state == CameraState::CAPTURING){
            return false;
        }
        else{
            state = CameraState::CAPTURING;
            photosTaken++;
            state = CameraState::IDLE;
            return true;
        }
    }

int CameraManager::getPhotosTaken() const {
    return photosTaken;
}