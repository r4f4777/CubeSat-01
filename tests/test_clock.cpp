
#include <cassert>
#include <iostream>
#include "MissionClock.h"

int main() {
    MissionClock clock;

    assert(clock.getTime() == 0);
    assert(clock.getSpeed() == 1);

    clock.tick();
    assert(clock.getTime() == 1);

    assert(clock.setSpeed(10));
    clock.tick();
    assert(clock.getTime() == 11);

    // Una velocidad inválida no debe cambiar el estado
    assert(!clock.setSpeed(50));
    assert(clock.getSpeed() == 10);

    clock.reset();
    assert(clock.getTime() == 0);
    assert(clock.getSpeed() == 1);

    std::cout << "MissionClock tests: PASSED\n";
    return 0;
}
