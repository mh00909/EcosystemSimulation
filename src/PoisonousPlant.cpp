#include "PoisonousPlant.h"


PoisonousPlant::PoisonousPlant(int x, int y, int sceneX, int sceneY)
        : Plant(x, y, sceneX, sceneY) {
   // energy = 100;
} // Trująca roślina może mieć mniejszą ilość energii

void PoisonousPlant::interact(Reserve* reserve) {

}
