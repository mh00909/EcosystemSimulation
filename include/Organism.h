#ifndef ECOSYSTEM_ORGANISM_H
#define ECOSYSTEM_ORGANISM_H

class Reserve;

class Organism {
public:
    Organism(int x, int y, int energy, int sceneX, int sceneY);
    virtual void move(int maxX, int maxY, int sceneX, int sceneY) = 0;
    virtual void interact(Reserve* reserve) = 0;
    virtual void updateEnergy() = 0;

    int getX() const;
    int getY() const;
    int getAge() const;
    int getEnergy() const;

    bool isAlive() const;
    bool canReproduce() const;


    void incrementAge();
    void ageAndConsumeEnergy(int energyLossPerStep);
    void setAlive(bool alive);
    void decreaseEnergy(int x);

protected:
    int x, y;
    bool alive;
    int age;
    int energy;
    int stageX, stageY;
    static const int reproductionAge = 5;
    static const int reproductionEnergy = 30;
};

#endif //ECOSYSTEM_ORGANISM_H
