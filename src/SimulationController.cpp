#include "SimulationController.h"
#include "Herbivore.h"
#include "Carnivore.h"
#include "Plant.h"
#include "PoisonousPlant.h"

SimulationController::SimulationController(int width, int height, int numHerbivores, int numCarnivores, int numPlants) {
    reserve = std::make_shared<Reserve>(width, height, numHerbivores, numCarnivores, numPlants);
}

void SimulationController::addOrganisms(int numHerbivores, int numCarnivores, int numPlants) {
    // Dodawanie nowych roślinożerców
    for (int i = 0; i < numHerbivores; ++i) {
        auto newHerbivore = std::make_unique<Herbivore>(rand() % reserve->getWidth(), rand() % reserve->getHeight(), reserve->getWidth(), reserve->getHeight());
        reserve->addHerbivore(std::move(newHerbivore));
    }
    // Dodawanie nowych drapieżników
    for (int i = 0; i < numCarnivores; ++i) {
        auto newCarnivore = std::make_unique<Carnivore>(rand() % reserve->getWidth(), rand() % reserve->getHeight(), reserve->getWidth(), reserve->getHeight());
        reserve->addCarnivore(std::move(newCarnivore));
    }
    // Dodawanie nowych roślin
    for (int i = 0; i < numPlants; ++i) {
        int x = rand();
        std::unique_ptr<Plant> newPlant;
        if (x % 2 == 0) {
            newPlant = std::make_unique<Plant>(rand() % reserve->getWidth(), rand() % reserve->getHeight(), reserve->getWidth(), reserve->getHeight());
        }
        else {
            newPlant = std::make_unique<PoisonousPlant>(rand() % reserve->getWidth(), rand() % reserve->getHeight(), reserve->getWidth(), reserve->getHeight());
        }

        reserve->addPlant(std::move(newPlant));
    }
}

void SimulationController::simulateStep() {
    if (reserve) {
        reserve->simulateStep();
    }
}

std::shared_ptr<Reserve> SimulationController::getReserve() const {
    return reserve;
}
