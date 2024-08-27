#include "Plant.h"

Plant::Plant(int x, int y, int sceneX, int sceneY)
        : Organism(x, y, 100, sceneX, sceneY) {
    if (x < 0) x = 0;
    if (x >= sceneX) x = sceneX - 1;
    if (y < 0) y = 0;
    if (y >= sceneY) y = sceneY - 1;
}

void Plant::move(int maxX, int maxY, int sceneX, int sceneY) {
    // w przypadku rozmnażania roślin, aby nowe rośliny nie pojawiały się poza sceną
    if (x < 0) x = 0;
    if (x >= sceneX) x = sceneX - 1;
    if (y < 0) y = 0;
    if (y >= sceneY) y = sceneY - 1;
}

void Plant::interact(Reserve* reserve) {
    // Rośliny nie wchodzą w interakcje, więc pozostaw puste lub nie implementuj
}

void Plant::updateEnergy() {
  //  energy -= 8;
    if (energy <= 0) alive = false;

    incrementAge();
}
