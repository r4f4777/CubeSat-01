#include "MissionClock.h"


int MissionClock::getTime() const{
    return currentTime;
}


int MissionClock::getSpeed() const{
    return speed;
}

bool MissionClock::setSpeed(int newSpeed) {
    if (newSpeed == 1 || newSpeed == 10 || newSpeed == 100) {
        speed = newSpeed;
        return true;
    }

    return false;
}

void MissionClock::tick() {
    currentTime += speed;
}


void MissionClock::reset(){
    currentTime = 0;
    speed = 1;
}