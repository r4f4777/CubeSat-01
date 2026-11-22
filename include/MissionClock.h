
#pragma once

class MissionClock {
private:
    int currentTime = 0;
    int speed = 1;

public:
    int getTime() const;
    int getSpeed() const;

    bool setSpeed(int newSpeed);
    void tick();
    void reset();
};
