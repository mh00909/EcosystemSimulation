#include "Reserve.h"
#include "PoisonousPlant.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <QDebug>
#include <thread>
#include <chrono>

// Konstruktor klasy Reserve, inicjalizujący rezerwat i dodający organizmy
Reserve::Reserve(int w, int h, int numHerbivores, int numCarnivores, int numPlants, int numScavengers)
        : QObject(nullptr), width(w), height(h) {

    srand(time(0));
    // Tworzenie roślinożerców
    for (int i = 0; i < numHerbivores; i++) {
        herbivores.push_back(std::make_unique<Herbivore>(rand() % width, rand() % height, width, height));
    }
    // Tworzenie drapieżników
    for (int i = 0; i < numCarnivores; i++) {
        carnivores.push_back(std::make_unique<Carnivore>(rand() % width, rand() % height, width, height));
    }

    for(int i=0; i < numScavengers; i++) {
        scavengers.push_back(std::make_unique<Scavenger>(rand() % width, rand() % height, width, height));
    }

    // Tworzenie roślin, losowo wybierając między zwykłą a trującą
    for (int i = 0; i < numPlants; i++) {
        if (rand() % 2 == 0) {  // Losowanie między zwykłą rośliną a trującą
            plants.push_back(std::make_unique<PoisonousPlant>(rand() % width, rand() % height, width, height));
        } else {
            plants.push_back(std::make_unique<Plant>(rand() % width, rand() % height, width, height));
        }
    }
}


void Reserve::simulateStep() {
    QMutexLocker locker(&mutex);

    int energyLossPerStep = 10;
    int aliveHerbivores = 0, aliveCarnivores = 0, aliveScavengers = 0;
    double totalAgeHerbivores = 0, totalEnergyHerbivores = 0;
    double totalAgeCarnivores = 0, totalEnergyCarnivores = 0;

    // Obsługa ruchu i starzenia się organizmów
    for (auto &herb : herbivores) {
        if (herb && herb->isAlive()) {
            herb->ageAndConsumeEnergy(energyLossPerStep);
            herb->move(max_organism_move, max_organism_move, width, height);
            aliveHerbivores++;
            totalAgeHerbivores += herb->getAge();
            totalEnergyHerbivores += herb->getEnergy();
        }
    }

    for (auto &carn : carnivores) {
        if (carn && carn->isAlive()) {
            carn->ageAndConsumeEnergy(energyLossPerStep);
            carn->move(max_organism_move, max_organism_move, width, height);
            aliveCarnivores++;
            totalAgeCarnivores += carn->getAge();
            totalEnergyCarnivores += carn->getEnergy();
        }
    }

    for (auto &scav : scavengers) {
        if (scav && scav->isAlive()) {
            scav->ageAndConsumeEnergy(energyLossPerStep);
            scav->move(max_organism_move, max_organism_move, width, height);
            aliveScavengers++;
        }
    }



    handleReproduction();
    handleInteractions();
    removeDeadOrganisms();

    bool allHerbivoresDeadOrEaten = std::all_of(herbivores.begin(), herbivores.end(), [](const auto& herb) {
        return herb != nullptr && (!herb->isAlive() || herb->isEatenByScavenger());
    });

    bool allCarnivoresDeadOrEaten = std::all_of(carnivores.begin(), carnivores.end(), [](const auto& carn) {
        return carn != nullptr && (!carn->isAlive() || carn->isEatenByScavenger());
    });

    bool allScavengersDeadOrEaten = std::all_of(scavengers.begin(), scavengers.end(), [](const auto& scav) {
        return scav != nullptr && (!scav->isAlive() || scav->isEatenByScavenger());
    });


    bool noAliveScavengers = aliveScavengers == 0;

    // Sprawdź, czy warunki zakończenia symulacji są spełnione
    if ((allHerbivoresDeadOrEaten && allCarnivoresDeadOrEaten && allScavengersDeadOrEaten) ||
        (noAliveScavengers && (aliveHerbivores == 0 && aliveCarnivores == 0))) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        emit simulationEnded();
    }
}

// Obsługa rozmnażania organizmów
void Reserve::handleReproduction() {
    std::vector<std::unique_ptr<Herbivore>> newHerbivores;
    std::vector<std::unique_ptr<Carnivore>> newCarnivores;
    std::vector<std::unique_ptr<Scavenger>> newScavengers;


    // Rozmnażanie roślinożerców
    for (auto &herb : herbivores) {
        if (herb && herb->isAlive()) {
            for (auto &otherHerb : herbivores) {
                if (herb != otherHerb && herb->canReproduce() && otherHerb->canReproduce() &&
                    herb->getX() > otherHerb->getX() - 20 && herb->getX() < otherHerb->getX() + 20 &&
                    herb->getY() > otherHerb->getY() - 20 && herb->getY() < otherHerb->getY() + 20) {

                    int newX = (herb->getX() + rand() % max_organism_move - max_organism_move / 2);
                    int newY = (herb->getY() + rand() % max_organism_move - max_organism_move / 2);
                    newHerbivores.push_back(std::make_unique<Herbivore>(newX, newY, width, height));

                    herb->decreaseEnergy(herb->getEnergy()/2);
                    otherHerb->decreaseEnergy(otherHerb->getEnergy()/2);

                    stats.birthsHerbivores++;
                }
            }
        }
    }
    herbivores.insert(herbivores.end(),
                      std::make_move_iterator(newHerbivores.begin()),
                      std::make_move_iterator(newHerbivores.end()));

    // Rozmnażanie drapieżników
    for (auto &carn : carnivores) {
        if (carn && carn->isAlive()) {
            for (auto &otherCarn : carnivores) {
                if (carn != otherCarn && carn->canReproduce() && otherCarn->canReproduce() &&
                    carn->getX() > otherCarn->getX() - 20 && carn->getX() < otherCarn->getX() + 20 &&
                    carn->getY() > otherCarn->getY() - 20 && carn->getY() < otherCarn->getY() + 20) {

                    int newX = (carn->getX() + rand() % max_organism_move - max_organism_move * 2);
                    int newY = (carn->getY() + rand() % max_organism_move - max_organism_move * 2);
                    newCarnivores.push_back(std::make_unique<Carnivore>(newX, newY, width, height));

                    carn->decreaseEnergy(carn->getEnergy()/2);
                    otherCarn->decreaseEnergy(carn->getEnergy()/2);

                    stats.birthsCarnivores++;
                }
            }
        }
    }
    carnivores.insert(carnivores.end(),
                      std::make_move_iterator(newCarnivores.begin()),
                      std::make_move_iterator(newCarnivores.end()));


    // Rozmnażanie padlinożerców
    for (auto &scav : scavengers) {
        if (scav && scav->isAlive()) {
            for (auto &otherScav : scavengers) {
                if (scav != otherScav && scav->canReproduce() && otherScav->canReproduce() &&
                scav->getX() > otherScav->getX() - 20 && scav->getX() < otherScav->getX() + 20 &&
                scav->getY() > otherScav->getY() - 20 && scav->getY() < otherScav->getY() + 20) {

                    int newX = (scav->getX() + rand() % max_organism_move - max_organism_move * 2);
                    int newY = (scav->getY() + rand() % max_organism_move - max_organism_move * 2);
                    newScavengers.push_back(std::make_unique<Scavenger>(newX, newY, width, height));

                    scav->decreaseEnergy(scav->getEnergy()/2);
                    otherScav->decreaseEnergy(scav->getEnergy()/2);

                    stats.birthScavengers++;
                }
            }
        }
    }
    scavengers.insert(scavengers.end(),
                      std::make_move_iterator(newScavengers.begin()),
                      std::make_move_iterator(newScavengers.end()));

}


// Obsługa interakcji między organizmami
void Reserve::handleInteractions() {
    for (auto &herb : herbivores) {
        if (herb && herb->isAlive()) {
            herb->interact(this);
            herb->updateEnergy();
        }
    }

    for (auto &carn : carnivores) {
        if (carn && carn->isAlive()) {
            carn->interact(this);
            carn->updateEnergy();
        }
    }

    for (auto &plant : plants) {
        if (plant && plant->isAlive()) {
            plant->updateEnergy();
        }
    }
    for (auto &scav : scavengers) {
        if (scav && scav->isAlive()) {
            scav->interact(this);
            scav->updateEnergy();
        }
    }
}


void Reserve::removeDeadOrganisms() {
    for (auto it = herbivores.begin(); it != herbivores.end();) {
        if (*it && !(*it)->isAlive() && (*it)->isEatenByScavenger()) {
            emit organismDied(it->get());
            it = herbivores.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = carnivores.begin(); it != carnivores.end();) {
        if (*it && !(*it)->isAlive() && (*it)->isEatenByScavenger()) {
            emit organismDied(it->get());
            it = carnivores.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = scavengers.begin(); it != scavengers.end();) {
        if (*it && !(*it)->isAlive() && (*it)->isEatenByScavenger()) {
            emit organismDied(it->get());
            it = scavengers.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = plants.begin(); it != plants.end();) {
        if (*it && !(*it)->isAlive()) {
            emit organismDied(it->get());
            it = plants.erase(it);
        } else {
            ++it;
        }
    }
}



// Dodawanie nowych organizmów do rezerwatu
void Reserve::addHerbivore(std::unique_ptr<Herbivore> herbivore) {
    if (herbivore) {
        herbivores.push_back(std::move(herbivore));
    }
}

void Reserve::addCarnivore(std::unique_ptr<Carnivore> carnivore) {
    if (carnivore) {
        carnivores.push_back(std::move(carnivore));
    }
}

void Reserve::addPlant(std::unique_ptr<Plant> plant) {
    if (plant) {
        plants.push_back(std::move(plant));
    }
}
void Reserve::addScavenger(std::unique_ptr<Scavenger> scavenger) {
    if (scavenger) {
        scavengers.push_back(std::move(scavenger));
    }
}

std::vector<std::unique_ptr<Herbivore>>& Reserve::getHerbivores() {
    return herbivores;
}

std::vector<std::unique_ptr<Carnivore>>& Reserve::getCarnivores() {
    return carnivores;
}

std::vector<std::unique_ptr<Plant>>& Reserve::getPlants() {
    return plants;
}

std::vector<std::unique_ptr<Scavenger>>& Reserve::getScavengers() {
    return scavengers;
}

int Reserve::getWidth() const {
    return width;
}
int Reserve::getHeight() const {
    return height;
}
void Reserve::clearAllOrganisms() {
    herbivores.clear();
    carnivores.clear();
    plants.clear();
}

Reserve::~Reserve(){}