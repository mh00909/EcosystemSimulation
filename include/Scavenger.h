#ifndef ECOSYSTEM_SCAVENGER_H
#define ECOSYSTEM_SCAVENGER_H

#include "Organism.h"

class Scavenger : public Organism {
    Scavenger(int x, int y, int sceneX, int sceneY);
    void interact(Reserve* reserve);
    void update() override;
};

#endif