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

void Carnivore::move(int maxX, int maxY, int sceneX, int sceneY, Reserve *reserve) {
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
    energy = 0.99 * energy;
    if (energy <= 0) alive = false;
    incrementAge();
}
