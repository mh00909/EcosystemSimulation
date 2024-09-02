#ifndef ECOSYSTEM_CARNIVORE_H
#define ECOSYSTEM_CARNIVORE_H

#include "Organism.h"
#include "Reserve.h"
#include "Herbivore.h"

class Herbivore;
/*
 * Klasa symulująca drapieżnika
 */
class Carnivore : public Organism {
public:
    Carnivore(int x, int y, int i, int i1);
    void move(int maxX, int maxY, int sceneX, int sceneY) override;
    void interact(Reserve* reserve) override;
    void updateEnergy() override;
};

#endif //ECOSYSTEM_CARNIVORE_H
