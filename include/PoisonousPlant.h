#ifndef ECOSYSTEM_POISONOUSPLANT_H
#define ECOSYSTEM_POISONOUSPLANT_H

#include "Reserve.h"
#include "Plant.h"


class PoisonousPlant : public Plant {
public:
    PoisonousPlant(int x, int y, int sceneX, int sceneY);
    void interact(Reserve* reserve) override;
    bool isPoisonous() const override { return true; }
};
#endif //ECOSYSTEM_POISONOUSPLANT_H
