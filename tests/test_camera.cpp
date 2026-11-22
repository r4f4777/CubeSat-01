
#include <cassert>
#include <iostream>
#include "CameraManager.h"

int main() {
    CameraManager camera;

    // Inicialmente no hay fotografías.
    assert(camera.getPhotosTaken() == 0);

    // Primera fotografía.
    assert(camera.takePhoto());
    assert(camera.getPhotosTaken() == 1);

    // Segunda fotografía.
    assert(camera.takePhoto());
    assert(camera.getPhotosTaken() == 2);

    std::cout << "CameraManager tests: PASSED\n";

    return 0;
}
