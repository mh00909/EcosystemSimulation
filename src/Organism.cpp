#include "Organism.h"

Organism::Organism(int posX, int posY, int initialEnergy, int sX, int sY)
        : x(posX), y(posY), energy(initialEnergy), alive(true), age(0), stageX(sX), stageY(sY) {}

int Organism::getX() const {
    return x;
}

int Organism::getY() const {
    return y;
}

bool Organism::isAlive() const {
    return alive;
}
void Organism::setAlive(bool a) {
    alive = a;
}

int Organism::getAge() const {
    return age;
}

void Organism::incrementAge() {
    ++age;
}
int Organism::getEnergy() const {
    return energy;
}
void Organism::ageAndConsumeEnergy(int energyLossPerStep) {
    incrementAge();
    energy -= energyLossPerStep;
    if (energy <= 0) {
        alive = false;
    }
}

bool Organism::canReproduce() const {
    return age >= reproductionAge && energy >= reproductionEnergy;
}
void Organism::decreaseEnergy(int x) {
    energy -= x;
}