#ifndef ECOSYSTEM_HERBIVORE_H
#define ECOSYSTEM_HERBIVORE_H

#include "Organism.h"
#include "Reserve.h"
/*
 * Klasa symulująca roślinożercę.
 */
class Herbivore : public Organism {
public:
    Herbivore(int x, int y, int i, int i1);
    void move(int maxX, int maxY, int sceneX, int sceneY, Reserve *reserve) override;
    void interact(Reserve* reserve) override;
    void updateEnergy() override;
    void searchForPlants(Reserve *reserve);
    void consumePlant(Reserve *reserve);
    void moveTo(int targetX, int targetY);
};

#endif //ECOSYSTEM_HERBIVORE_H
