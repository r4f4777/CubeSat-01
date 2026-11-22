
#include <cassert>
#include <iostream>
#include "SensorValidator.h"

int main() {
    SensorValidator sensor;

    // Una lectura válida no genera un fallo.
    assert(sensor.processReading(75));
    assert(!sensor.hasFault());

    // Primera lectura incorrecta.
    assert(!sensor.processReading(-10));
    assert(!sensor.hasFault());

    // Segunda lectura incorrecta.
    assert(!sensor.processReading(150));
    assert(!sensor.hasFault());

    // Tercera lectura incorrecta: activa el fallo.
    assert(!sensor.processReading(200));
    assert(sensor.hasFault());

    // Cinco lecturas válidas consecutivas
    for (int i = 0; i < 8; i++){
        assert(sensor.processReading(75));
    }

    // El sensor debería haberse recuperado
    assert(!sensor.hasFault());

    std::cout << "SensorValidator tests: PASSED\n";

    return 0;
}
