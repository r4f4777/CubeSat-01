
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include "OrbitSimulator.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: orbit_cli <time_seconds>\n";
        return 1;
    }

    double timeSeconds;

    try {
        timeSeconds = std::stod(argv[1]);
    } catch (...) {
        std::cerr << "ERROR: Invalid time\n";
        return 1;
    }

    if (!std::isfinite(timeSeconds) || timeSeconds < 0) {
        std::cerr << "ERROR: Time must be a finite positive value\n";
        return 1;
    }

    OrbitSimulator orbit(51.6);
    OrbitalPosition position = orbit.getPosition(timeSeconds);

    std::cout << std::setprecision(15);

    std::cout << "{\n";
    std::cout << "  \"time\": " << timeSeconds << ",\n";
    std::cout << "  \"x\": " << position.x << ",\n";
    std::cout << "  \"y\": " << position.y << ",\n";
    std::cout << "  \"z\": " << position.z << "\n";
    std::cout << "}\n";

    return 0;
}
