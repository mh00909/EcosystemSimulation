#include <gtest/gtest.h>
#include "Plant.h"
#include "Organism.h"
#include "Herbivore.h"

class Herbivore;
class Organism;
class Herbivore;

TEST(OrganismTest, Initialization) {
    Plant plant(10, 20, 100, 100);

    EXPECT_EQ(plant.getX(), 10);
    EXPECT_EQ(plant.getY(), 20);
    EXPECT_EQ(plant.getEnergy(), 100);
    EXPECT_TRUE(plant.isAlive());
}

TEST(OrganismTest, MovementRestrictions) {
    Herbivore herbivore(150, 150, 100, 100);

    // Sprawdzenie ograniczenia poruszania się w obrębie sceny
    herbivore.moveTo(200, 200);
    EXPECT_EQ(herbivore.getX(), 99); // Ograniczenie pozycji X do rozmiaru sceny
    EXPECT_EQ(herbivore.getY(), 99); // Ograniczenie pozycji Y do rozmiaru sceny
}

// Test aktualizacji energii
TEST(OrganismTest, EnergyConsumption) {
    Plant plant(10, 10, 100, 100);

    plant.updateEnergy();  // Zakładamy, że energia jest zmniejszana w updateEnergy
    EXPECT_FALSE(plant.isAlive());  // Roślina umiera, gdy energia osiąga zero
}
