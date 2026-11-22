
#include <cassert>
#include <iostream>
#include "MissionManager.h"

int main() {
    MissionManager mission("mission_test.dat");;

    // Primera orden válida
    assert(mission.schedulePhoto({"MADRID", 30, 75}));
    assert(mission.getCommandState() == CommandState::PENDING);

    // No debe permitir una segunda orden activa
    assert(!mission.schedulePhoto({"LONDON", 40, 90}));

    // Iniciar la ejecución de la misión
    assert(mission.beginExecution());
    assert(mission.getCommandState() == CommandState::EXECUTING);

    // No debe iniciarse dos veces
    assert(!mission.beginExecution());

    // Completar la misión
    assert(mission.completeCommand());
    assert(mission.getCommandState() == CommandState::COMPLETED);

    // No debe completarse dos veces
    assert(!mission.completeCommand());

    // Simular un reinicio creando otro MissionManager
    MissionManager restarted("mission_test.dat");

    // Recuperar la misión desde mission.dat
    assert(restarted.loadCommand());

    // Debe conservar el estado COMPLETED
    assert(restarted.getCommandState() == CommandState::COMPLETED);

    // No debe poder ejecutarse nuevamente
    assert(!restarted.beginExecution());


    // Crear una nueva misión
    MissionManager expiredMission("mission_test.dat");

    assert(expiredMission.schedulePhoto({"SEVILLA", 10, 30}));

    // Simular que ha pasado su ventana temporal
    assert(expiredMission.hasExpired(40));

    // Registrar la caducidad
    assert(expiredMission.expireCommand());
    assert(expiredMission.getCommandState() == CommandState::EXPIRED);

    // Simular otro reinicio
    MissionManager restartedExpired("mission_test.dat");

    assert(restartedExpired.loadCommand());
    assert(restartedExpired.getCommandState() == CommandState::EXPIRED);

    // No debe ejecutarse una misión caducada
    assert(!restartedExpired.beginExecution());


    // Crear una misión que queda en EXECUTING
    MissionManager uncertainMission("mission_test.dat");

    assert(uncertainMission.schedulePhoto({"MADRID", 50, 100}));
    assert(uncertainMission.beginExecution());

    // Simular reinicio sin completar la orden
    MissionManager restartedUncertain("mission_test.dat");

    assert(restartedUncertain.loadCommand());

    // Debe conservar EXECUTING
    assert(restartedUncertain.getCommandState() ==
           CommandState::EXECUTING);

    // El resultado requiere revisión
    assert(restartedUncertain.needsReview());

    // No se debe iniciar otra vez
    assert(!restartedUncertain.beginExecution());

    std::cout << "MissionManager tests: PASSED\n";

    return 0;
}
