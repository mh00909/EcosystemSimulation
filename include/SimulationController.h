#ifndef ECOSYSTEM_SIMULATIONCONTROLLER_H
#define ECOSYSTEM_SIMULATIONCONTROLLER_H


#include "Reserve.h"
#include <memory>

/*
 * Klasa zarządzająca główną logiką symulacji ekosystemu.
 */

class SimulationController {
public:
    SimulationController(int width, int height, int numHerbivores, int numCarnivores, int numPlants, int numScavengers);

    void addOrganisms(int numHerbivores, int numCarnivores, int numPlants, int numScavengers);
    void simulateStep(); // wykonuje jeden krok symulacji

    std::shared_ptr<Reserve> getReserve() const;

private:
    std::shared_ptr<Reserve> reserve;
};


#endif //ECOSYSTEM_SIMULATIONCONTROLLER_H
