
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include "OrbitSimulator.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: orbit_samples_cli <start> <step> <count>\n";
        return 1;
    }

    double start, step;
    int count;

    try {
        std::size_t pos;

        start = std::stod(argv[1], &pos);
        if (pos != std::string(argv[1]).size())
            return 1;

        step = std::stod(argv[2], &pos);
        if (pos != std::string(argv[2]).size())
            return 1;

        count = std::stoi(argv[3], &pos);
        if (pos != std::string(argv[3]).size())
            return 1;
    } catch (...) {
        std::cerr << "ERROR: Invalid arguments\n";
        return 1;
    }

    if (!std::isfinite(start) ||
        !std::isfinite(step) ||
        start < 0 ||
        step <= 0 ||
        count < 2 ||
        count > 1000 ||
        start + step * (count - 1) > 1e9) {

        std::cerr << "ERROR: Invalid sampling parameters\n";
        return 1;
    }

    OrbitSimulator orbit(51.6);

    std::cout << std::setprecision(15);
    std::cout << "[\n";

    for (int i = 0; i < count; i++) {
        double time = start + i * step;

        OrbitalPosition position = orbit.getPosition(time);

        std::cout << "  {"
                  << "\"time\":" << time << ","
                  << "\"x\":" << position.x << ","
                  << "\"y\":" << position.y << ","
                  << "\"z\":" << position.z
                  << "}";

        if (i < count - 1) {
            std::cout << ",";
        }

        std::cout << "\n";
    }

    std::cout << "]\n";

    return 0;
}
