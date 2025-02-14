#ifndef ECOSYSTEM_PLANT_H
#define ECOSYSTEM_PLANT_H

#include "Organism.h"

/*
 * Klasa symulująca roślinę.
 */
class Plant : public Organism {
public:

    Plant(int x, int y, int i, int i1);

    void move(int maxX, int maxY, int sceneX, int sceneY, Reserve * reserve) override;
    void interact(Reserve* reserve)override;
    void updateEnergy() override;
    virtual bool isPoisonous() const { return false; }
};

#endif //ECOSYSTEM_PLANT_H
