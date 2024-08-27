#include <cstdlib>
#include <QDebug>
#include "Carnivore.h"
#include "Herbivore.h"

Carnivore::Carnivore(int x, int y, int sceneX, int sceneY)
        : Organism(x, y, 100, sceneX, sceneY) {
    if (x < 0) x = 0;
    if (x >= sceneX) x = sceneX - 1;
    if (y < 0) y = 0;
    if (y >= sceneY) y = sceneY - 1;
}

void Carnivore::move(int maxX, int maxY, int sceneX, int sceneY) {
    x = x + rand() % maxX - maxX/2;
    y = y + rand() % maxY - maxY/2;

    if (x < 0) x = 0;
    if (x >= sceneX) x = sceneX - 1;
    if (y < 0) y = 0;
    if (y >= sceneY) y = sceneY - 1;
}

void Carnivore::interact(Reserve* reserve) {
    for (auto &herb : reserve->getHerbivores()) {
        if (herb->isAlive() &&
            herb->getX() < x + 20 && herb->getX() > x - 20 &&
            herb->getY() < y + 20 && herb->getY() > x - 20)
        {
            reserve->stats.herbivoresEaten++;
            energy += 50;
            herb->setAlive(false);
            break;
        }
    }
}

void Carnivore::updateEnergy() {
    energy -= 10;
    if (energy <= 0) alive = false;
    incrementAge();
}
