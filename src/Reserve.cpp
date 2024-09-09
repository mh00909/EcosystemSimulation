#include "Reserve.h"
#include "PoisonousPlant.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <QDebug>
#include <thread>
#include <chrono>
#include <random>

// Konstruktor klasy Reserve, inicjalizujący rezerwat i dodający organizmy
Reserve::Reserve(int w, int h, int numHerbivores, int numCarnivores, int numPlants, int numScavengers)
        : QObject(nullptr), width(w), height(h) {

    initializeTerrain();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distWidth(0, width - 1);
    std::uniform_int_distribution<> distHeight(0, height - 1);
    std::uniform_int_distribution<> distPlantType(0, 1);



    // Tworzenie roślinożerców
    for (int i = 0; i < numHerbivores; i++) {
        int x = distWidth(gen);
        int y = distHeight(gen);

        TerrainType terrainType = getTerrainType(x, y);
        while (terrainType != TerrainType::Grassland) {
            x = distWidth(gen);
            y = distHeight(gen);
            terrainType = getTerrainType(x, y);
        }
        herbivores.push_back(std::make_unique<Herbivore>(x, y, width, height));
    }
    // Tworzenie drapieżników
    for (int i = 0; i < numCarnivores; i++) {
        int x = distWidth(gen);
        int y = distHeight(gen);
        TerrainType terrainType = getTerrainType(x, y);
        while (terrainType != TerrainType::Grassland) {
            x = distWidth(gen);
            y = distHeight(gen);
            terrainType = getTerrainType(x, y);
        }
        carnivores.push_back(std::make_unique<Carnivore>(x, y, width, height));
    }

    for(int i=0; i < numScavengers; i++) {
        int x = distWidth(gen);
        int y = distHeight(gen);
        TerrainType terrainType = getTerrainType(x, y);
        while (terrainType != TerrainType::Grassland) {
            x = distWidth(gen);
            y = distHeight(gen);
            terrainType = getTerrainType(x, y);
        }
        scavengers.push_back(std::make_unique<Scavenger>(x, y, width, height));
    }

    // Tworzenie roślin, losowo wybierając między zwykłą a trującą
    for (int i = 0; i < numPlants; i++) {
        int x = distWidth(gen);
        int y = distHeight(gen);

        TerrainType terrainType = getTerrainType(x, y);
        while (terrainType != TerrainType::Grassland) {
            x = distWidth(gen);
            y = distHeight(gen);
            terrainType = getTerrainType(x, y);
        }

        if (distPlantType(gen) == 0) {  // Losowanie między zwykłą rośliną a trującą
            plants.push_back(std::make_unique<PoisonousPlant>(x, y, width, height));
        } else {
            plants.push_back(std::make_unique<Plant>(x, y, width, height));
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
            herb->ageAndConsumeEnergy(energyLossPerStep);  // Starzenie się i zużycie energii
            herb->searchForPlants(this);  // Znajdowanie rośliny
            herb->move(max_organism_move, max_organism_move, width, height, this);  // Ruch
            herb->consumePlant(this);  // Konsumowanie rośliny, jeśli jest na tym samym polu
            aliveHerbivores++;  // Zliczanie żywych roślinożerców
            totalAgeHerbivores += herb->getAge();
            totalEnergyHerbivores += herb->getEnergy();
        }
    }


    for (auto &carn : carnivores) {
        if (carn && carn->isAlive()) {
            carn->ageAndConsumeEnergy(energyLossPerStep);
            carn->move(max_organism_move, max_organism_move, width, height, this);
            aliveCarnivores++;
            totalAgeCarnivores += carn->getAge();
            totalEnergyCarnivores += carn->getEnergy();
        }
    }

    for (auto &scav : scavengers) {
        if (scav && scav->isAlive()) {
            scav->ageAndConsumeEnergy(energyLossPerStep);
            scav->move(max_organism_move, max_organism_move, width, height, this);
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

        } else if (*it && !(*it)->isAlive() && !(*it)->updatedOrganisms) {

            stats.deathsHerbivores++;
            (*it)->updatedOrganisms = true;
            ++it;

        }
        else {
            ++it;
        }
    }

    for (auto it = carnivores.begin(); it != carnivores.end();) {

        if (*it && !(*it)->isAlive() && (*it)->isEatenByScavenger()) {

            emit organismDied(it->get());
            it = carnivores.erase(it);

        } else if (*it && !(*it)->isAlive() && !(*it)->updatedOrganisms) {
            stats.deathsCarnivores++;
            (*it)->updatedOrganisms = true;
            ++it;
        } else {
            ++it;
        }
    }

    for (auto it = scavengers.begin(); it != scavengers.end();) {
        if (*it && !(*it)->isAlive() && (*it)->isEatenByScavenger()) {
            emit organismDied(it->get());
            it = scavengers.erase(it);
        }
        else if (*it && !(*it)->isAlive() && !(*it)->updatedOrganisms) {
            stats.deathScavengers++;
            (*it)->updatedOrganisms = true;
            ++it;
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

void Reserve::initializeTerrain() {
    terrainGrid.resize(height, std::vector<TerrainType>(width, TerrainType::Grassland));

    std::random_device rd;
    std::mt19937 gen(rd());

    addRandomArea(gen, TerrainType::Forest, 2);
    addRandomArea(gen, TerrainType::Water, 5);
    addRandomArea(gen, TerrainType::Mountain, 5);
}

void Reserve::addRandomArea(std::mt19937 &gen, TerrainType terrainType, int areaCount) {
    std::uniform_int_distribution<> posXDist(0, width - 1);
    std::uniform_int_distribution<> posYDist(0, height - 1);
    //std::uniform_int_distribution<> sizeDist(3, 10);

    for (int i = 0; i < areaCount; ++i) {
        // Losuj pozycję i rozmiar prostokąta
        int startX = posXDist(gen);
        int startY = posYDist(gen);

        int areaWidth = 200;
        int areaHeight = 200;

        if (terrainType == TerrainType::Mountain) {
            areaWidth = 200;
            areaHeight = 80;
        }
        if (terrainType == TerrainType::Water) {
            areaWidth = 200;
            areaHeight = 100;
        }
        if (terrainType == TerrainType::Forest) {
            areaWidth = 250;
            areaHeight = 250;
        }

        // Zapełnij obszar typem terenu
        for (int y = startY; y < startY + areaHeight && y < height; ++y) {
            for (int x = startX; x < startX + areaWidth && x < width; ++x) {
                terrainGrid[y][x] = terrainType;
            }
        }
    }
}
TerrainType Reserve::getTerrainType(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return TerrainType::Grassland;
    }
    return terrainGrid[y][x];
}