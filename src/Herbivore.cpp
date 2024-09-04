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

void Herbivore::move(int maxX, int maxY, int sceneX, int sceneY) {
    x = x + rand() % maxX - maxX/2;
    y = y + rand() % maxY - maxY/2;

    if (x < 0) x = 0;
    if (x >= sceneX) x = sceneX - 1;
    if (y < 0) y = 0;
    if (y >= sceneY) y = sceneY - 1;
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
