#ifndef ECOSYSTEM_SCAVENGER_H
#define ECOSYSTEM_SCAVENGER_H

#include "Organism.h"
/*
 * Klasa symulująca padlinożercę.
 */
class Scavenger : public Organism {
public:
    Scavenger(int x, int y, int width, int height);

    void move(int max, int y, int sceneX, int sceneY, Reserve *reserve) override;
    void updateEnergy() override;
    void interact(Reserve* reserve) override;
    bool isAlive() const ;


private:
    int scavengingRange;
};

#endif