#include "Reserve.h"
#include "PoisonousPlant.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <QDebug>
#include <thread>
#include <chrono>

// Konstruktor klasy Reserve, inicjalizujący rezerwat i dodający organizmy
Reserve::Reserve(int w, int h, int numHerbivores, int numCarnivores, int numPlants)
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
    QMutexLocker locker(&mutex); // Blokuje mutex w tej funkcji
    int energyLossPerStep = 10;

    // Obsługa ruchu i starzenia się organizmów
    for (auto &herb : herbivores) {
        if (herb->isAlive()) {
            herb->ageAndConsumeEnergy(energyLossPerStep);
            herb->move(max_organism_move, max_organism_move, width, height);
        }
    }
    for (auto &carn : carnivores) {
        if (carn->isAlive()) {
            carn->ageAndConsumeEnergy(energyLossPerStep);
            carn->move(max_organism_move, max_organism_move, width, height);
        }
    }

    // Obsługa rozmnażania i interakcji między organizmami
    handleReproduction();
    handleInteractions();
    removeDeadOrganisms();

    bool allHerbivoresDead = std::all_of(herbivores.begin(), herbivores.end(), [](const std::unique_ptr<Herbivore>& herb) {
        return !herb->isAlive();
    });

    bool allCarnivoresDead = std::all_of(carnivores.begin(), carnivores.end(), [](const std::unique_ptr<Carnivore>& carn) {
        return !carn->isAlive();
    });
    // Jeśli wszystkie są martwe, emitujemy sygnał zakończenia symulacji
    if (allHerbivoresDead && allCarnivoresDead) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        emit simulationEnded(); // Emitowanie sygnału do MainWindow, aby zakończyć symulację
    }
}

// Obsługa rozmnażania organizmów
void Reserve::handleReproduction() {
    std::vector<std::unique_ptr<Herbivore>> newHerbivores;
    std::vector<std::unique_ptr<Carnivore>> newCarnivores;
    std::vector<std::unique_ptr<Plant>> newPlants;

    // Rozmnażanie roślinożerców
    for (auto &herb : herbivores) {
        if (herb->isAlive()) {
            for (auto &otherHerb : herbivores) {
                if (herb != otherHerb && herb->canReproduce() && otherHerb->canReproduce() &&
                    herb->getX() > otherHerb->getX() - 20 && herb->getX() < otherHerb->getX() + 20 &&
                    herb->getY() > otherHerb->getY() - 20 && herb->getY() < otherHerb->getY() + 20) {

                    int newX = (herb->getX() + rand() % max_organism_move - max_organism_move / 2);
                    int newY = (herb->getY() + rand() % max_organism_move - max_organism_move / 2);
                    newHerbivores.push_back(std::make_unique<Herbivore>(newX, newY, width, height));

                    herb->decreaseEnergy(20);
                    otherHerb->decreaseEnergy(20);

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
        if (carn->isAlive()) {
            for (auto &otherCarn : carnivores) {
                if (carn != otherCarn && carn->canReproduce() && otherCarn->canReproduce() &&
                    carn->getX() > otherCarn->getX() - 20 && carn->getX() < otherCarn->getX() + 20 &&
                    carn->getY() > otherCarn->getY() - 20 && carn->getY() < otherCarn->getY() + 20) {

                    int newX = (carn->getX() + rand() % max_organism_move - max_organism_move * 2);
                    int newY = (carn->getY() + rand() % max_organism_move - max_organism_move * 2);
                    newCarnivores.push_back(std::make_unique<Carnivore>(newX, newY, width, height));

                    carn->decreaseEnergy(5);
                    otherCarn->decreaseEnergy(5);

                    stats.birthsCarnivores++;
                }
            }
        }
    }
    carnivores.insert(carnivores.end(),
                      std::make_move_iterator(newCarnivores.begin()),
                      std::make_move_iterator(newCarnivores.end()));


}


// Obsługa interakcji między organizmami
void Reserve::handleInteractions() {
    for (auto &herb : herbivores) {
        if (herb->isAlive()) {
            herb->interact(this);
            herb->updateEnergy();
        }
    }

    for (auto &carn : carnivores) {
        if (carn->isAlive()) {
            carn->interact(this);
            carn->updateEnergy();
        }
    }

    for (auto &plant : plants) {
        if (plant->isAlive()) {
            plant->updateEnergy();
        }
    }
}


// Usuwanie martwych organizmów z rezerwatu
void Reserve::removeDeadOrganisms() {
    for (auto it = herbivores.begin(); it != herbivores.end(); ) {
        if (!(*it)->isAlive()) {
            emit organismDied(it->get()); // Emituje sygnał, że organizm umarł
            stats.deathsHerbivores++;
            it = herbivores.erase(it); // Usuwa organizm z wektora
        } else {
            ++it;
        }
    }

    for (auto it = carnivores.begin(); it != carnivores.end(); ) {
        if (!(*it)->isAlive()) {
            emit organismDied(it->get());
            stats.deathsCarnivores++;
            it = carnivores.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = plants.begin(); it != plants.end(); ) {
        if (!(*it)->isAlive()) {
            emit organismDied(it->get());
            it = plants.erase(it);
        } else {
            ++it;
        }
    }
}



// Dodawanie nowych organizmów do rezerwatu
void Reserve::addHerbivore(std::unique_ptr<Herbivore> herbivore) {
    herbivores.push_back(std::move(herbivore));
}

void Reserve::addCarnivore(std::unique_ptr<Carnivore> carnivore) {
    carnivores.push_back(std::move(carnivore));
}

void Reserve::addPlant(std::unique_ptr<Plant> plant) {
    plants.push_back(std::move(plant));
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

int Reserve::getWidth() const {
    return width;
}
int Reserve::getHeight() const {
    return height;
}

Reserve::~Reserve(){}