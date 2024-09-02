#ifndef ECOSYSTEM_ORGANISM_H
#define ECOSYSTEM_ORGANISM_H

class Reserve;

/*
 * Klasa bazowa dla wszystkich organizmów w ekosystemie.
 */

class Organism {
public:
    Organism(int x, int y, int energy, int sceneX, int sceneY);

    // Metody wirtualne do nadpisania przez klasy pochodne
    virtual void move(int maxX, int maxY, int sceneX, int sceneY) = 0;
    virtual void interact(Reserve* reserve) = 0;
    virtual void updateEnergy() = 0;

    // Metody dostępu
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


    bool isDead() const;
    bool isEatenByScavenger() const;
    void markAsDead();
    void setEaten();

protected:
    int x, y;
    bool alive;
    int age;
    int energy;
    int stageX, stageY; // wymiary sceny symulacji
    static const int reproductionAge = 5;
    static const int reproductionEnergy = 30;

    bool dead;
    bool eaten;
};

#endif //ECOSYSTEM_ORGANISM_H
