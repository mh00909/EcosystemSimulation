#ifndef ECOSYSTEM_SIMULATIONCONTROLLER_H
#define ECOSYSTEM_SIMULATIONCONTROLLER_H


#include "Reserve.h"
#include <memory>

class SimulationController {
public:
    SimulationController(int width, int height, int numHerbivores, int numCarnivores, int numPlants);

    void addOrganisms(int numHerbivores, int numCarnivores, int numPlants);
    void simulateStep();

    std::shared_ptr<Reserve> getReserve() const;

private:
    std::shared_ptr<Reserve> reserve;  // Wskaźnik zarządzający rezerwą
};


#endif //ECOSYSTEM_SIMULATIONCONTROLLER_H
