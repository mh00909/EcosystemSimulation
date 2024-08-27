#include "MainWindow.h"
#include "InteractiveOrganism.h"
#include "PoisonousPlant.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QGraphicsEllipseItem>
#include <QKeyEvent>
#include <QDir>
#include <QObject>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), simulationController(nullptr), isPaused(false), simulationDuration(0), simulationEnded(false), isRestarting(false) {

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    startButton = new QPushButton("Start Simulation", this);
    stopButton = new QPushButton("Stop Simulation", this);
    showStatsButton = new QPushButton("Show Statistics", this);
    restartButton = new QPushButton("Restart Simulation", this);

    herbivoreInput = new QSpinBox(this);
    herbivoreInput->setRange(0, 100);
    herbivoreInput->setValue(15);
    carnivoreInput = new QSpinBox(this);
    carnivoreInput->setRange(0, 100);
    carnivoreInput->setValue(5);
    plantInput = new QSpinBox(this);
    plantInput->setRange(0, 100);
    plantInput->setValue(20);

    herbivoreLabel = new QLabel("Herbivores:", this);
    carnivoreLabel = new QLabel("Carnivores:", this);
    plantLabel = new QLabel("Plants:", this);

    layout->addWidget(herbivoreLabel);
    layout->addWidget(herbivoreInput);
    layout->addWidget(carnivoreLabel);
    layout->addWidget(carnivoreInput);
    layout->addWidget(plantLabel);
    layout->addWidget(plantInput);

    herbivoreCountLabel = new QLabel("Herbivores: 0", this);
    carnivoreCountLabel = new QLabel("Carnivores: 0", this);
    plantCountLabel = new QLabel("Plants: 0", this);

    layout->addWidget(herbivoreCountLabel);
    layout->addWidget(carnivoreCountLabel);
    layout->addWidget(plantCountLabel);
    herbivoreCountLabel->setVisible(false);
    carnivoreCountLabel->setVisible(false);
    plantCountLabel->setVisible(false);

    layout->addWidget(startButton);
    layout->addWidget(stopButton);
    layout->addWidget(showStatsButton);
    layout->addWidget(restartButton);

    showStatsButton->setVisible(false);
    restartButton->setVisible(false);

    newHerbivoresInput = new QSpinBox(this);
    newHerbivoresInput->setRange(0, 100);
    newHerbivoresInput->setValue(0);

    newCarnivoresInput = new QSpinBox(this);
    newCarnivoresInput->setRange(0, 100);
    newCarnivoresInput->setValue(0);

    newPlantsInput = new QSpinBox(this);
    newPlantsInput->setRange(0, 100);
    newPlantsInput->setValue(0);

    newHerbivoreLabel = new QLabel("New Herbivores:", this);
    newCarnivoreLabel = new QLabel("New Carnivores:", this);
    newPlantsLabel = new QLabel("New Plants:", this);

    addOrganismsButton = new QPushButton("Add Organisms", this);

    layout->addWidget(newHerbivoreLabel);
    layout->addWidget(newHerbivoresInput);
    layout->addWidget(newCarnivoreLabel);
    layout->addWidget(newCarnivoresInput);
    layout->addWidget(newPlantsLabel);
    layout->addWidget(newPlantsInput);
    layout->addWidget(addOrganismsButton);
    newHerbivoresInput->setVisible(false);
    newCarnivoresInput->setVisible(false);
    newPlantsInput->setVisible(false);
    newHerbivoreLabel->setVisible(false);
    newCarnivoreLabel->setVisible(false);
    newPlantsLabel->setVisible(false);
    addOrganismsButton->setVisible(false);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    scene->setBackgroundBrush(Qt::darkGreen);
    layout->addWidget(view);

    setCentralWidget(centralWidget);

    simulationController = new SimulationController(700, 450, 0, 0, 0);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartSimulation);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopSimulation);
    connect(showStatsButton, &QPushButton::clicked, this, &MainWindow::onShowStatsButtonClicked);
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::onRestartButtonClicked);
    connect(addOrganismsButton, &QPushButton::clicked, this, &MainWindow::onAddOrganismsClicked);


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onSimulationStepCompleted);
    connect(simulationController->getReserve().get(), &Reserve::simulationEnded, this, &MainWindow::onSimulationEnded);

    scene->setSceneRect(0, 0, 800, 700);
}

MainWindow::~MainWindow() {
    delete simulationController;
}

void MainWindow::onAddOrganismsClicked() {
    int newHerbivores = newHerbivoresInput->value();
    int newCarnivores = newCarnivoresInput->value();
    int newPlants = newPlantsInput->value();

    simulationController->addOrganisms(newHerbivores, newCarnivores, newPlants);
    updateScene();
    updateCounts();

    newHerbivoresInput->setValue(0);
    newCarnivoresInput->setValue(0);
    newPlantsInput->setValue(0);
}

void MainWindow::onStartSimulation() {
    qDebug() << "on start simulation";
    // Zatrzymanie symulacji, jeśli jest aktywna
    if (timer->isActive()) {
        timer->stop();
    }

    // Jeśli była wciśnięta pauza
    if (isPaused) {
        timer->start(1000);
        isPaused = false;
        startButton->setVisible(false);
        stopButton->setVisible(true);

        newHerbivoresInput->setVisible(false);
        newCarnivoresInput->setVisible(false);
        newPlantsInput->setVisible(false);
        newHerbivoreLabel->setVisible(false);
        newCarnivoreLabel->setVisible(false);
        newPlantsLabel->setVisible(false);
        addOrganismsButton->setVisible(false);

        plantLabel->setVisible(false);
        herbivoreLabel->setVisible(false);
        carnivoreLabel->setVisible(false);

        herbivoreCountLabel->setVisible(true);
        carnivoreCountLabel->setVisible(true);
        plantCountLabel->setVisible(true);

        return;
    }
    if(isRestarting) {
        isRestarting = false;
        timer->start(1000);
        isPaused = false;


        startButton->setVisible(false);
        stopButton->setVisible(true);

        newHerbivoresInput->setVisible(false);
        newCarnivoresInput->setVisible(false);
        newPlantsInput->setVisible(false);
        newHerbivoreLabel->setVisible(false);
        newCarnivoreLabel->setVisible(false);
        newPlantsLabel->setVisible(false);
        addOrganismsButton->setVisible(false);

        plantInput->setVisible(false);
        herbivoreInput->setVisible(false);
        carnivoreInput->setVisible(false);
        plantLabel->setVisible(false);
        herbivoreLabel->setVisible(false);
        carnivoreLabel->setVisible(false);

        herbivoreCountLabel->setVisible(true);
        carnivoreCountLabel->setVisible(true);
        plantCountLabel->setVisible(true);
    }


    isPaused = false;
    startButton->setVisible(false);
    // Czyszczenie sceny i usuwanie wszystkich organizmów
    scene->clear();
    organismItems.clear();

    int numHerbivores = herbivoreInput->value();
    int numCarnivores = carnivoreInput->value();
    int numPlants = plantInput->value();

    
    delete simulationController;
    simulationController = new SimulationController(700, 450, numHerbivores, numCarnivores, numPlants);

    simulationTimer.restart();

    // Uruchomienie timera do symulacji
    if (!timer->isActive()) {
        timer->start(1000);
    }

    stopButton->setText("Pause");
    startButton->setText("Resume");

    herbivoreInput->setVisible(false);
    carnivoreInput->setVisible(false);
    plantInput->setVisible(false);
    herbivoreLabel->setVisible(false);
    carnivoreLabel->setVisible(false);
    plantLabel->setVisible(false);

    herbivoreCountLabel->setVisible(true);
    carnivoreCountLabel->setVisible(true);
    plantCountLabel->setVisible(true);

    showStatsButton->setVisible(false);
    restartButton->setVisible(false);

    newHerbivoresInput->setVisible(false);
    newCarnivoresInput->setVisible(false);
    newPlantsInput->setVisible(false);
    addOrganismsButton->setVisible(false);

    newHerbivoreLabel->setVisible(false);
    newCarnivoreLabel->setVisible(false);
    newPlantsLabel->setVisible(false);
}
void MainWindow::onRestartButtonClicked() {
    // Zatrzymanie symulacji, jeśli jest aktywna
    if (timer->isActive()) {
        timer->stop();
    }
    qDebug() << "on restart button clicked";
    isRestarting = true;
    simulationEnded = false;
    isPaused = false;

    // Resetowanie GUI
    scene->clear();
    organismItems.clear();
    simulationTimer.restart();

    showStatsButton->setVisible(false);
    restartButton->setVisible(false);

    herbivoreInput->setVisible(true);
    carnivoreInput->setVisible(true);
    plantInput->setVisible(true);
    herbivoreLabel->setVisible(true);
    carnivoreLabel->setVisible(true);
    plantLabel->setVisible(true);

    herbivoreCountLabel->setVisible(false);
    carnivoreCountLabel->setVisible(false);
    plantCountLabel->setVisible(false);
    newHerbivoresInput->setVisible(false);
    newCarnivoresInput->setVisible(false);
    newPlantsInput->setVisible(false);
    newHerbivoreLabel->setVisible(false);
    newCarnivoreLabel->setVisible(false);
    newPlantsLabel->setVisible(false);
    addOrganismsButton->setVisible(false);

    startButton->setVisible(true);
    stopButton->setVisible(true);

    // Inicjalizacja nowej symulacji z nowymi parametrami
    int numHerbivores = herbivoreInput->value();
    int numCarnivores = carnivoreInput->value();
    int numPlants = plantInput->value();

    delete simulationController;  // Usunięcie starego kontrolera
    simulationController = new SimulationController(700, 450, numHerbivores, numCarnivores, numPlants);
    connect(simulationController->getReserve().get(), &Reserve::simulationEnded, this, &MainWindow::onSimulationEnded);

    // Aktualizacja sceny i liczników
    updateScene();
    updateCounts();
}

void MainWindow::onStopSimulation() {
    if (!isPaused) {
            timer->stop();
            isPaused = true;

    } else {
        onStartSimulation();
    }

    herbivoreInput->setVisible(false);
    carnivoreInput->setVisible(false);
    plantInput->setVisible(false);

    stopButton->setVisible(false);

    if(!simulationEnded) {
        startButton->setVisible(true);
        newHerbivoresInput->setVisible(true);
        newCarnivoresInput->setVisible(true);
        newPlantsInput->setVisible(true);
        addOrganismsButton->setVisible(true);

        newHerbivoreLabel->setVisible(true);
        newCarnivoreLabel->setVisible(true);
        newPlantsLabel->setVisible(true);
    }

}

void MainWindow::onSimulationEnded() {
    simulationEnded = true;
    onStopSimulation();

    simulationDuration = simulationTimer.elapsed() / 1000;  // Czas w sekundach

    QMessageBox::information(this, "Simulation Ended", "All herbivores and carnivores are dead. The simulation has ended.");

    newHerbivoresInput->setVisible(false);
    newCarnivoresInput->setVisible(false);
    newPlantsInput->setVisible(false);
    newHerbivoreLabel->setVisible(false);
    newCarnivoreLabel->setVisible(false);
    newPlantsLabel->setVisible(false);
    addOrganismsButton->setVisible(false);

    showStatsButton->setVisible(true);
    restartButton->setVisible(true);
    startButton->setVisible(false);
    stopButton->setVisible(false);
}

void MainWindow::onSimulationStepCompleted() {
    simulationController->simulateStep();
    updateScene();
    updateCounts();
    if (simulationController->getReserve()->getHerbivores().empty() &&
        simulationController->getReserve()->getCarnivores().empty()) {

        onSimulationEnded();
    }
}

void MainWindow::updateScene() {
    auto reserve = simulationController->getReserve();

    // Usuwanie nieżyjących organizmów ze sceny
    for (auto it = organismItems.begin(); it != organismItems.end();) {
        if (!it->first->isAlive()) {
            scene->removeItem(it->second);
            delete it->second;
            it = organismItems.erase(it);
        } else {
            ++it;
        }
    }

    // Aktualizacja pozycji i dodawanie nowych organizmów na scenę
    QPixmap plantPixmap(":/gui/img/flower.png");
    QPixmap poisonousPlantPixmap(":/gui/img/poisonous.png");
    QPixmap herbivorePixmap(":/gui/img/deer.png");
    QPixmap carnivorePixmap(":/gui/img/wolf.png");

    int organismSize = 30; // Rozmiar obrazu organizmów

    for (const auto &plant : reserve->getPlants()) {
        if (plant->isAlive()) {
            if (organismItems.count(plant.get())) {
                // Aktualizacja pozycji istniejącego organizmu
                if (organismItems[plant.get()]->pos() != QPointF(plant->getX(), plant->getY())) {
                    organismItems[plant.get()]->setPos(plant->getX(), plant->getY());
                }
            } else {
                // Dodanie nowego organizmu do sceny
                QPixmap *currentPixmap = plant->isPoisonous() ? &poisonousPlantPixmap : &plantPixmap;
                auto plantItem = new InteractiveOrganism(*currentPixmap, plant.get(), organismSize, organismSize);
                organismItems[plant.get()] = plantItem;
                scene->addItem(plantItem);
            }
        }
    }

    for (const auto &herb : reserve->getHerbivores()) {
        if (herb->isAlive()) {
            if (organismItems.count(herb.get())) {
                if (organismItems[herb.get()]->pos() != QPointF(herb->getX(), herb->getY())) {
                    organismItems[herb.get()]->setPos(herb->getX(), herb->getY());
                }
            } else {
                auto herbivoreItem = new InteractiveOrganism(herbivorePixmap, herb.get(), organismSize, organismSize);
                organismItems[herb.get()] = herbivoreItem;
                scene->addItem(herbivoreItem);
            }
        }
    }

    for (const auto &carn : reserve->getCarnivores()) {
        if (carn->isAlive()) {
            if (organismItems.count(carn.get())) {
                if (organismItems[carn.get()]->pos() != QPointF(carn->getX(), carn->getY())) {
                    organismItems[carn.get()]->setPos(carn->getX(), carn->getY());
                }
            } else {
                auto carnivoreItem = new InteractiveOrganism(carnivorePixmap, carn.get(), organismSize, organismSize);
                organismItems[carn.get()] = carnivoreItem;
                scene->addItem(carnivoreItem);
            }
        }
    }
}


void MainWindow::updateCounts() {
    int herbivoreCount = simulationController->getReserve()->getHerbivores().size();
    int carnivoreCount = simulationController->getReserve()->getCarnivores().size();
    int plantCount = simulationController->getReserve()->getPlants().size();

    herbivoreCountLabel->setText(QString("Herbivores: %1").arg(herbivoreCount));
    carnivoreCountLabel->setText(QString("Carnivores: %1").arg(carnivoreCount));
    plantCountLabel->setText(QString("Plants: %1").arg(plantCount));
}

void MainWindow::onShowStatsButtonClicked() {
    // Pobranie statystyk symulacji z kontrolera
    SimulationStats stats = simulationController->getReserve()->getStats();

    // Tworzenie wiadomości ze statystykami
    QString statsMessage = QString("Simulation Duration: %1 seconds\n").arg(simulationDuration);
    statsMessage += QString(
                                   "Herbivores born: %1\n"
                                   "Carnivores born: %2\n"
                                   "Herbivores died: %3\n"
                                   "Carnivores died: %4\n"
                                   "Plants eaten: %5\n"
                                   "Herbivores eaten by carnivores: %6\n"
                                   "Herbivores poisoned by plants: %7")
            .arg(stats.birthsHerbivores)
            .arg(stats.birthsCarnivores)
            .arg(stats.deathsHerbivores)
            .arg(stats.deathsCarnivores)
            .arg(stats.plantsEaten)
            .arg(stats.herbivoresEaten)
            .arg(stats.herbivoresPoisoned);

    QMessageBox::information(this, "Simulation Stats", statsMessage);
}



void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_P) {
        onStopSimulation();
    } else if (event->key() == Qt::Key_R) {
        onStartSimulation();
    }
}

