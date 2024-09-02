#ifndef ECOSYSTEM_RESERVE_H
#define ECOSYSTEM_RESERVE_H

#include <vector>
#include <memory>
#include <QObject>
#include <QMutex>
#include "Plant.h"
#include "Herbivore.h"
#include "Carnivore.h"
#include "Scavenger.h"

class Carnivore;
class Herbivore;
class Scavenger;

// Przechowuje statystyki symulacji
struct SimulationStats {
    int birthsHerbivores = 0;
    int birthsCarnivores = 0;
    int birthScavengers = 0;
    int deathsHerbivores = 0;
    int deathsCarnivores = 0;
    int deathScavengers = 0;

    int plantsEaten = 0;
    int herbivoresPoisoned=0;
    int herbivoresEaten = 0;
    int eatenByScavengers = 0;


};

// Klasa Reserve reprezentuje rezerwat, w którym odbywa się symulacja
class Reserve : public QObject {
    Q_OBJECT
public:
    Reserve(int width, int height, int numHerbivores, int numCarnivores, int numPlants, int numScavengers);
    ~Reserve() override;

    // Metoda symulująca jeden krok symulacji
    void simulateStep();

    // Funkcje odpowiedzialne za rozmanżanie, interakcje między organizmami, usuwanie martwych organizmów
    void handleReproduction();
    void handleInteractions();
    void removeDeadOrganisms();



    // Metody zwracające referencje do wektorów z organizmami
    std::vector<std::unique_ptr<Herbivore>>& getHerbivores();
    std::vector<std::unique_ptr<Carnivore>>& getCarnivores();
    std::vector<std::unique_ptr<Plant>>& getPlants();
    std::vector<std::unique_ptr<Scavenger>>& getScavengers();

    // Szerokość, wysokość rezerwatu
    int getWidth() const;
    int getHeight() const;

    // Zmienna przechowująca statystyki symulacji
    SimulationStats stats;
    const SimulationStats& getStats() const { return stats; }

    // Metody dodające organizmy do rezerwatu
    void addHerbivore(std::unique_ptr<Herbivore> herbivore);
    void addCarnivore(std::unique_ptr<Carnivore> carnivore);
    void addPlant(std::unique_ptr<Plant> plant);
    void addScavenger(std::unique_ptr<Scavenger> scavenger);

    void clearAllOrganisms();

signals:
        void simulationEnded();  // Sygnał emitowany, gdy symulacja się kończy
        void organismDied(Organism* organism); // Sygnał emitowany, gdy organizm umiera
private:
    int width, height; // wymiary rezerwatu
    int max_organism_move = 60; // Maksymalny ruch organizmu
    std::vector<std::unique_ptr<Herbivore>> herbivores;
    std::vector<std::unique_ptr<Carnivore>> carnivores;
    std::vector<std::unique_ptr<Plant>> plants;
    std::vector<std::unique_ptr<Scavenger>> scavengers;

    QMutex mutex; // Mutex do synchronizacji dostępu do danych
};


#endif
