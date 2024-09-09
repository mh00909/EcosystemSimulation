#include <cstdlib>
#include <QDebug>
#include "Herbivore.h"
#include "PoisonousPlant.h"

Herbivore::Herbivore(int x, int y, int sceneX, int sceneY)
        : Organism(x, y, 120, sceneX, sceneY) {
    if (x < 0) x = 0;
    if (x >= sceneX) x = sceneX - 1;
    if (y < 0) y = 0;
    if (y >= sceneY) y = sceneY - 1;
}

void Herbivore::move(int maxX, int maxY, int sceneX, int sceneY, Reserve* reserve) {
    int newX = x + rand() % maxX - maxX/2;
    int newY = y + rand() % maxY - maxY/2;

    if (x < 0) x = 0;
    if (x >= sceneX) x = sceneX - 1;
    if (y < 0) y = 0;
    if (y >= sceneY) y = sceneY - 1;

    // Pobierz typ terenu w nowej pozycji
    TerrainType terrain = reserve->getTerrainType(newX, newY);

    // Sprawdź wpływ terenu na ruch
    switch (terrain) {
        case TerrainType::Water:
            // Roślinożercy nie mogą wejść na wodę
            return;  // Anuluj ruch
        case TerrainType::Mountain:
            // Ruch po górach jest wolniejszy
            if (rand() % 2 == 0) {
                return;  // Zmniejszona szansa na ruch
            }
            break;
        case TerrainType::Forest:
            // Ruch przez las może być trudniejszy lub wolniejszy
            // Można tu dodać logikę
            break;
        default:
            break;
    }

    // Aktualizuj pozycję organizmu
    x = newX;
    y = newY;
}

void Herbivore::interact(Reserve* reserve) {
    for (auto &plant : reserve->getPlants()) {
        if (plant->isAlive() &&
            x - 20 < plant->getX() && plant->getX() < x + 20 &&
            y - 20 < plant->getY() && plant->getY() < y + 20) {

            bool isPoisonousPlant = plant.get()->isPoisonous();

            if (isPoisonousPlant) { // trująca roślina zabija roślinożercę
                reserve->stats.herbivoresPoisoned++;
                this->setAlive(false);
            } else { // nietrująca roślina dodaje energii
                reserve->stats.plantsEaten++;
                energy += 20;
                plant->setAlive(false);
            }

            break;
        }
    }
}


void Herbivore::updateEnergy() {
    energy = 0.99 * energy;
    if (energy <= 0) alive = false;
    incrementAge();
}

void Herbivore::searchForPlants(Reserve *reserve) {
    Plant* closestPlant = nullptr;
    int minDistance = std::numeric_limits<int>::max();

    // Przeszukiwanie listy roślin w rezerwacie
    for (auto& plant : reserve->getPlants()) {
        if (plant->isAlive()) {
            int distance = std::abs(plant->getX() - x) + std::abs(plant->getY() - y);
            if (distance < minDistance) {
                closestPlant = plant.get();
                minDistance = distance;
            }
        }
    }

    // Jeśli znaleziono roślinę, herbivore przemieszcza się w jej kierunku
    if (closestPlant) {
        moveTo(closestPlant->getX(), closestPlant->getY());
    }
}

void Herbivore::consumePlant(Reserve *reserve) {
    Plant* plantToEat = nullptr;

    // Sprawdź, czy roślina znajduje się w tym samym miejscu, co roślinożerca
    for (auto& plant : reserve->getPlants()) {
        if (plant->isAlive() && plant->getX() == x && plant->getY() == y) {
            plantToEat = plant.get();
            break;
        }
    }

    // Jeśli znaleziono roślinę na tym samym polu
    if (plantToEat) {
        if (plantToEat->isPoisonous()) {
            // Roślinożerca zjada trującą roślinę i umiera
            reserve->stats.herbivoresPoisoned++;
            this->setAlive(false);
        } else {
            // Roślinożerca zjada roślinę i zyskuje energię
            reserve->stats.plantsEaten++;
            energy += 20;  // Przyznaj energię roślinożercy
            plantToEat->setAlive(false);  // Roślina staje się martwa
        }
    }
}


void Herbivore::moveTo(int targetX, int targetY) {
    if (x < targetX) {
        x++;
    }
    else if (x > targetX) {
        x--;
    }
    if (y < targetY) {
        y++;
    }
    else if (y > targetY) {
        y--;
    }
}