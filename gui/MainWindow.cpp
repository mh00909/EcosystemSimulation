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
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

using namespace QtCharts;


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),  isPaused(false), simulationDuration(0), simulationEnded(false), isRestarting(false) {

    // Central widget, layout
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // Elementy UI
    startButton = new QPushButton("Start Simulation", this);
    stopButton = new QPushButton("Stop Simulation", this);
    showStatsButton = new QPushButton("Show Statistics", this);
    restartButton = new QPushButton("Restart Simulation", this);


    // Układ poziomy dla przycisków + i -
    QWidget *zoomWidget = new QWidget(this);
    zoomWidget->setFixedSize(100, 40);
    QHBoxLayout *zoomLayout = new QHBoxLayout(zoomWidget);
    zoomLayout->setSpacing(0);
    zoomLayout->setContentsMargins(0, 0, 0, 0);
    QToolButton *zoomInButton = new QToolButton();
    zoomInButton->setText("+");
    QToolButton *zoomOutButton = new QToolButton(this);
    zoomOutButton->setText("-");
    zoomInButton->setFixedSize(30, 30);
    zoomOutButton->setFixedSize(30, 30);
    zoomLayout->addWidget(zoomInButton);
    zoomLayout->addWidget(zoomOutButton);

    herbivoreInput = new QSpinBox(this);
    herbivoreInput->setRange(0, 100);
    herbivoreInput->setValue(15);
    carnivoreInput = new QSpinBox(this);
    carnivoreInput->setRange(0, 100);
    carnivoreInput->setValue(5);
    plantInput = new QSpinBox(this);
    plantInput->setRange(0, 100);
    plantInput->setValue(20);
    scavengerInput = new QSpinBox(this);
    scavengerInput->setRange(0, 100);
    scavengerInput->setValue(10);


    herbivoreLabel = new QLabel("Herbivores:", this);
    carnivoreLabel = new QLabel("Carnivores:", this);
    plantLabel = new QLabel("Plants:", this);
    scavengersLabel = new QLabel("Scavengers:", this);

    leftLayout->addWidget(zoomWidget);
    leftLayout->addWidget(herbivoreLabel);
    leftLayout->addWidget(herbivoreInput);
    leftLayout->addWidget(carnivoreLabel);
    leftLayout->addWidget(carnivoreInput);
    leftLayout->addWidget(plantLabel);
    leftLayout->addWidget(plantInput);
    leftLayout->addWidget(scavengersLabel);
    leftLayout->addWidget(scavengerInput);

    herbivoreCountLabel = new QLabel("Herbivores: 0", this);
    carnivoreCountLabel = new QLabel("Carnivores: 0", this);
    plantCountLabel = new QLabel("Plants: 0", this);
    scavengerCountLabel = new QLabel("Scavengers: 0", this);

    leftLayout->addWidget(herbivoreCountLabel);
    leftLayout->addWidget(carnivoreCountLabel);
    leftLayout->addWidget(plantCountLabel);
    leftLayout->addWidget(scavengerCountLabel);
    herbivoreCountLabel->setVisible(false);
    carnivoreCountLabel->setVisible(false);
    plantCountLabel->setVisible(false);
    scavengerCountLabel->setVisible(false);

    leftLayout->addWidget(startButton);
    leftLayout->addWidget(stopButton);
    leftLayout->addWidget(showStatsButton);
    leftLayout->addWidget(restartButton);


    showStatsButton->setVisible(false);
    restartButton->setVisible(false);

    // Dodawanie organizmów w trakcie symulacji
    newHerbivoresInput = new QSpinBox(this);
    newHerbivoresInput->setRange(0, 100);
    newHerbivoresInput->setValue(0);

    newCarnivoresInput = new QSpinBox(this);
    newCarnivoresInput->setRange(0, 100);
    newCarnivoresInput->setValue(0);

    newPlantsInput = new QSpinBox(this);
    newPlantsInput->setRange(0, 100);
    newPlantsInput->setValue(0);

    newScavengersInput = new QSpinBox(this);
    newScavengersInput->setRange(0, 100);
    newScavengersInput->setValue(0);

    newHerbivoreLabel = new QLabel("New Herbivores:", this);
    newCarnivoreLabel = new QLabel("New Carnivores:", this);
    newPlantsLabel = new QLabel("New Plants:", this);
    newScavengerLabel = new QLabel("New Scavengers:", this);

    addOrganismsButton = new QPushButton("Add Organisms", this);

    // Suwak do ustawiania prędkości symulacji
    speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(1, 20);
    speedSlider->setValue(1);
    speedLabel = new QLabel("Simulation Speed", this);

    leftLayout->addWidget(newHerbivoreLabel);
    leftLayout->addWidget(newHerbivoresInput);
    leftLayout->addWidget(newCarnivoreLabel);
    leftLayout->addWidget(newCarnivoresInput);
    leftLayout->addWidget(newPlantsLabel);
    leftLayout->addWidget(newPlantsInput);
    leftLayout->addWidget(newScavengerLabel);
    leftLayout->addWidget(newScavengersInput);
    leftLayout->addWidget(speedLabel);
    leftLayout->addWidget(speedSlider);
    leftLayout->addWidget(addOrganismsButton);

    newHerbivoresInput->setVisible(false);
    newCarnivoresInput->setVisible(false);
    newPlantsInput->setVisible(false);
    newScavengersInput->setVisible(false);
    newHerbivoreLabel->setVisible(false);
    newCarnivoreLabel->setVisible(false);
    newPlantsLabel->setVisible(false);
    newScavengerLabel->setVisible(false);
    addOrganismsButton->setVisible(false);




    // scena, widok
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 970, 800);

    view = new QGraphicsView(scene, this);
    QPixmap backgroundPixmap(":/gui/img/back.png");
    scene->setBackgroundBrush(QBrush(backgroundPixmap.scaled(scene->sceneRect().size().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    leftLayout->addWidget(view);

    initializeChart();

    mainLayout->addLayout(leftLayout, 5);
    mainLayout->addWidget(chartView, 2);

    setCentralWidget(centralWidget);

    initializeSimulationController();

    initializeAxisRange();

    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartSimulation);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopSimulation);
    connect(showStatsButton, &QPushButton::clicked, this, &MainWindow::onShowStatsButtonClicked);
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::onRestartButtonClicked);
    connect(addOrganismsButton, &QPushButton::clicked, this, &MainWindow::onAddOrganismsClicked);
    connect(speedSlider, &QSlider::valueChanged,  this, &MainWindow::setSimulationSpeed);
    connect(zoomInButton, &QToolButton::clicked, this, &MainWindow::zoomIn);
    connect(zoomOutButton, &QToolButton::clicked, this, &MainWindow::zoomOut);



    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onSimulationStepCompleted);

    if (auto reserve = simulationController->getReserve()) {
        connect(reserve.get(), &Reserve::simulationEnded, this, &MainWindow::onSimulationEnded);
    }


}
void MainWindow::initializeSimulationController() {
    int numHerbivores = herbivoreInput->value();
    int numCarnivores = carnivoreInput->value();
    int numPlants = plantInput->value();
    int numScavengers = scavengerInput->value();

    simulationController = std::make_unique<SimulationController>(1280, 720, numHerbivores, numCarnivores, numPlants, numScavengers);
}

void MainWindow::onAddOrganismsClicked() {
    int newHerbivores = newHerbivoresInput->value();
    int newCarnivores = newCarnivoresInput->value();
    int newPlants = newPlantsInput->value();
    int newScavengers = newScavengersInput->value();

    simulationController->addOrganisms(newHerbivores, newCarnivores, newPlants, newScavengers);
    updateScene();
    updateCounts();

    newHerbivoresInput->setValue(0);
    newCarnivoresInput->setValue(0);
    newPlantsInput->setValue(0);
    newScavengersInput->setValue(0);
}

void MainWindow::onStartSimulation() {
    if (timer->isActive()) {
        timer->stop();
    }

    int numHerbivores = herbivoreInput->value();
    int numCarnivores = carnivoreInput->value();
    int numPlants = plantInput->value();
    int numScavengers = scavengerInput->value();

    if (isRestarting) {
        isRestarting = false;
        isPaused = false;

        simulationController = std::make_unique<SimulationController>(1200, 700, numHerbivores, numCarnivores, numPlants, numScavengers);
        simulationTimer.restart();
        updateUIForSimulationRunning();

        timer->start(1000 / speedSlider->value());
        updateScene();
        updateCounts();
        return;
    }

    if (isPaused) {
        isPaused = false;
        updateUIForSimulationRunning();
        timer->start(1000 / speedSlider->value());
        return;
    }

    simulationController = std::make_unique<SimulationController>(1200, 700, numHerbivores, numCarnivores, numPlants, numScavengers);
    simulationTimer.restart();
    updateUIForSimulationRunning();
    timer->start(1000 / speedSlider->value());
    updateScene();
    updateCounts();
}



void MainWindow::onRestartButtonClicked() {
    if (timer->isActive()) {
        timer->stop();
    }

    isRestarting = true;
    simulationEnded = false;
    isPaused = false;

    scene->clear();
    organismItems.clear();

    simulationController->getReserve()->clearAllOrganisms();
    simulationTimer.restart();

    herbivoreInput->setValue(0);
    carnivoreInput->setValue(0);
    plantInput->setValue(0);
    scavengerInput->setValue(0);

    herbivoreInput->setEnabled(true);
    carnivoreInput->setEnabled(true);
    plantInput->setEnabled(true);
    scavengerInput->setEnabled(true);

    resetUIForNewSimulation();

    herbivoreSeries->clear();
    carnivoreSeries->clear();
    plantSeries->clear();
    scavengerSeries->clear();

    int numHerbivores = herbivoreInput->value();
    int numCarnivores = carnivoreInput->value();
    int numPlants = plantInput->value();
    int numScavengers = scavengerInput->value();

    simulationController = std::make_unique<SimulationController>(1200, 700, numHerbivores, numCarnivores, numPlants, numScavengers);


    if (auto reserve = simulationController->getReserve()) {
        QObject::disconnect(reserve.get(), &Reserve::simulationEnded, this, &MainWindow::onSimulationEnded);
        connect(reserve.get(), &Reserve::simulationEnded, this, &MainWindow::onSimulationEnded, Qt::UniqueConnection);
    }

    updateScene();
    updateCounts();

    startButton->setVisible(true);
    stopButton->setVisible(false);
}


void MainWindow::onStopSimulation() {
    if (!isPaused) {
        timer->stop();
        isPaused = true;

        startButton->setText("Resume");
        startButton->setVisible(true);
        stopButton->setVisible(false);

        newHerbivoresInput->setVisible(true);
        newCarnivoresInput->setVisible(true);
        newPlantsInput->setVisible(true);
        newScavengersInput->setVisible(true);
        addOrganismsButton->setVisible(true);

        newHerbivoreLabel->setVisible(true);
        newCarnivoreLabel->setVisible(true);
        newPlantsLabel->setVisible(true);
        newScavengerLabel->setVisible(true);
    } else {
        onStartSimulation();
    }
}

void MainWindow::onSimulationEnded() {
    if (simulationEnded) return;
    simulationEnded = true;

    if (timer->isActive()) {
        timer->stop();
    }

    simulationDuration = simulationTimer.elapsed() / 1000;
    QMessageBox::information(this, "Simulation Ended", "All animals are dead. The simulation has ended.");

    updateUIForSimulationEnded();


    showStatsButton->setVisible(true);
    restartButton->setVisible(true);
    startButton->setVisible(false);
    stopButton->setVisible(false);
}




void MainWindow::onSimulationStepCompleted() {
    if (simulationController) {
        simulationController->simulateStep();
    }

    updateScene();
    updateCounts();

    int step = simulationTimer.elapsed() / 1000;

    if (simulationController && simulationController->getReserve()) {
        // Liczba żywych roślinożerców
        int aliveHerbivores = std::count_if(simulationController->getReserve()->getHerbivores().begin(),
                                            simulationController->getReserve()->getHerbivores().end(),
                                            [](const auto& herb) { return herb->isAlive(); });

        // Liczba żywych drapieżników
        int aliveCarnivores = std::count_if(simulationController->getReserve()->getCarnivores().begin(),
                                            simulationController->getReserve()->getCarnivores().end(),
                                            [](const auto& carn) { return carn->isAlive(); });

        // Liczba żywych roślin
        int alivePlants = std::count_if(simulationController->getReserve()->getPlants().begin(),
                                        simulationController->getReserve()->getPlants().end(),
                                        [](const auto& plant) { return plant->isAlive(); });

        // Liczba żywych padlinożerców
        int aliveScavengers = std::count_if(simulationController->getReserve()->getScavengers().begin(),
                                            simulationController->getReserve()->getScavengers().end(),
                                            [](const auto& scav) { return scav->isAlive(); });

        // Aktualizacja wykresu
        herbivoreSeries->append(step, aliveHerbivores);
        carnivoreSeries->append(step, aliveCarnivores);
        plantSeries->append(step, alivePlants);
        scavengerSeries->append(step, aliveScavengers);
    }

    adjustAxisRange();
    chart->update();

    auto reserve = simulationController->getReserve();
    if (
                (std::all_of(reserve->getScavengers().begin(), reserve->getScavengers().end(), [](const auto& scav) { return !scav->isAlive(); } )&&
                (std::all_of(reserve->getCarnivores().begin(), reserve->getCarnivores().end(), [](const auto& carn) { return !carn->isAlive(); }) &&
                        (std::all_of(reserve->getHerbivores().begin(), reserve->getHerbivores().end(), [](const auto& herb) { return !herb->isAlive(); }))
                )
            ||
            (reserve->getHerbivores().empty() && reserve->getCarnivores().empty() && (reserve->getScavengers().empty() || std::all_of(reserve->getScavengers().begin(), reserve->getScavengers().end(), [](const auto& scav) { return !scav->isAlive(); })))
            ) )
            {
            onSimulationEnded();
    }
}

void MainWindow::updateScene() {
    auto reserve = simulationController->getReserve();

    if (!reserve) {
        return;
    }

    scene->clear();
    organismItems.clear();


    static QPixmap forestPixmap(":/gui/img/forest.png");
    static QPixmap waterPixmap(":/gui/img/lake.png");
    static QPixmap mountainPixmap(":/gui/img/mountains.png");


    int terrainHeight = 200, terrainWidth = 200;
    for (int y = 0; y < reserve->getHeight(); y++) {
        for (int x = 0; x < reserve->getWidth(); x++) {
            TerrainType terrain = reserve->getTerrainType(x, y);
            QPixmap pixmap;

            switch (terrain) {
                case TerrainType::Forest:
                    pixmap = forestPixmap;
                    terrainWidth = 250;
                    terrainHeight = 250;
                    break;
                case TerrainType::Water:
                    pixmap = waterPixmap;
                    terrainHeight = 100;
                    terrainWidth = 200;
                    break;
                case TerrainType::Mountain:
                    pixmap = mountainPixmap;
                    terrainHeight = 80;
                    terrainWidth = 200;
                    break;
                default:
                    continue;
            }

            QGraphicsPixmapItem *terrainItem = scene->addPixmap(pixmap.scaled(terrainWidth, terrainHeight));
            terrainItem->setPos(x , y );
            x += terrainWidth;
        }
        y += terrainHeight;
    }


    static QPixmap plantPixmap(":/gui/img/flower.png");
    static QPixmap poisonousPlantPixmap(":/gui/img/poisonous.png");
    static QPixmap herbivorePixmap(":/gui/img/deer.png");
    static QPixmap carnivorePixmap(":/gui/img/wolf.png");
    static QPixmap scavengerPixmap(":/gui/img/hyena.png");
    static QPixmap deadHerbivorePixmap(":/gui/img/dead_deer.png");
    static QPixmap deadCarnivorePixmap(":/gui/img/dead_lion.png");
    static QPixmap deadScavengerPixmap(":/gui/img/dead_hyena.png");

    int organismSize = 50;

    // Usunięcie martwych organizmów
    for (auto it = organismItems.begin(); it != organismItems.end();) {
        if (it->first && !it->first->isAlive() && it->first->isEatenByScavenger()) {
            scene->removeItem(it->second);
            delete it->second;
            it = organismItems.erase(it);
        } else {
            ++it;
        }
    }


    auto updateOrAddOrganism = [&](auto& organisms, QPixmap& livePixmap, QPixmap& deadPixmap) {
        for (const auto& organism : organisms) {
            if (organism) {
                QPixmap pixmap = organism->isAlive() ? livePixmap : deadPixmap;
                if (!organism->isAlive() && organism->isEatenByScavenger()) {
                    continue;
                }
                auto it = organismItems.find(organism.get());
                if (it != organismItems.end()) {
                    it->second->setPixmap(pixmap);
                    it->second->setPos(organism->getX(), organism->getY());
                } else if (organism->isAlive() || (!organism->isAlive() && !organism->isEatenByScavenger())) {
                    auto item = new InteractiveOrganism(pixmap, organism.get(), organismSize, organismSize);

                    organismItems[organism.get()] = item;
                    scene->addItem(item);


                }
            }
        }
    };

    updateOrAddOrganism(reserve->getHerbivores(), herbivorePixmap, deadHerbivorePixmap);
    updateOrAddOrganism(reserve->getCarnivores(), carnivorePixmap, deadCarnivorePixmap);
    updateOrAddOrganism(reserve->getScavengers(), scavengerPixmap, deadScavengerPixmap);

    for (const auto& plant : reserve->getPlants()) {
        QPixmap& pixmap = dynamic_cast<PoisonousPlant*>(plant.get()) ? poisonousPlantPixmap : plantPixmap;

        if (plant && plant->isAlive()) {
            auto it = organismItems.find(plant.get());
            if (it != organismItems.end()) {
                // Aktualizacja położenia
                if (it->second->pos() != QPointF(plant->getX(), plant->getY())) {
                    it->second->setPos(plant->getX(), plant->getY());
                }
            } else {
                // Dodanie nowej rośliny
                auto plantItem = new InteractiveOrganism(pixmap, plant.get(), organismSize, organismSize);
                organismItems[plant.get()] = plantItem;
                scene->addItem(plantItem);
            }
        }
    }
}

void MainWindow::updateCounts() {
    // Liczba żywych roślinożerców
    int herbivoreCount = std::count_if(simulationController->getReserve()->getHerbivores().begin(),
                                       simulationController->getReserve()->getHerbivores().end(),
                                       [](const auto& herb) { return herb->isAlive(); });

    // Liczba żywych drapieżników
    int carnivoreCount = std::count_if(simulationController->getReserve()->getCarnivores().begin(),
                                       simulationController->getReserve()->getCarnivores().end(),
                                       [](const auto& carn) { return carn->isAlive(); });

    // Liczba żywych roślin
    int plantCount = std::count_if(simulationController->getReserve()->getPlants().begin(),
                                   simulationController->getReserve()->getPlants().end(),
                                   [](const auto& plant) { return plant->isAlive(); });

    // Liczba żywych padlinożerców
    int scavengerCount = std::count_if(simulationController->getReserve()->getScavengers().begin(),
                                       simulationController->getReserve()->getScavengers().end(),
                                       [](const auto& scav) { return scav->isAlive(); });


    herbivoreCountLabel->setText(QString("Herbivores: %1").arg(herbivoreCount));
    carnivoreCountLabel->setText(QString("Carnivores: %1").arg(carnivoreCount));
    plantCountLabel->setText(QString("Plants: %1").arg(plantCount));
    scavengerCountLabel->setText(QString("Scavengers: %1").arg(scavengerCount));
}


void MainWindow::onShowStatsButtonClicked() {
    SimulationStats stats = simulationController->getReserve()->getStats();

    // Tworzenie wiadomości ze statystykami
    QString statsMessage = QString("Simulation Duration: %1 seconds\n").arg(simulationDuration);
    statsMessage += QString(
            "Herbivores born: %1\n"
            "Carnivores born: %2\n"
            "Scavengers born: %3\n"
            "Herbivores died: %4\n"
            "Carnivores died: %5\n"
            "Scavengers died: %6\n"
            "Plants eaten: %7\n"
            "Herbivores eaten by carnivores: %8\n"
            "Herbivores poisoned by plants: %9\n"
            "Dead animals eaten by scavengers: %10")
            .arg(stats.birthsHerbivores)
            .arg(stats.birthsCarnivores)
            .arg(stats.birthScavengers)
            .arg(stats.deathsHerbivores)
            .arg(stats.deathsCarnivores)
            .arg(stats.deathScavengers)
            .arg(stats.plantsEaten)
            .arg(stats.herbivoresEaten)
            .arg(stats.herbivoresPoisoned)
            .arg(stats.eatenByScavengers);

    QMessageBox::information(this, "Simulation Stats", statsMessage);



}


void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_P) {
        onStopSimulation();
    } else if (event->key() == Qt::Key_R) {
        onStartSimulation();
    }
}

void MainWindow::updateUIForSimulationRunning() {
    herbivoreInput->setVisible(false);
    carnivoreInput->setVisible(false);
    plantInput->setVisible(false);
    scavengerInput->setVisible(false);
    herbivoreLabel->setVisible(false);
    carnivoreLabel->setVisible(false);
    plantLabel->setVisible(false);
    scavengersLabel->setVisible(false);

    herbivoreCountLabel->setVisible(true);
    carnivoreCountLabel->setVisible(true);
    plantCountLabel->setVisible(true);
    scavengerCountLabel->setVisible(true);

    showStatsButton->setVisible(false);
    restartButton->setVisible(false);

    newHerbivoresInput->setVisible(false);
    newCarnivoresInput->setVisible(false);
    newPlantsInput->setVisible(false);
    newScavengersInput->setVisible(false);
    addOrganismsButton->setVisible(false);

    newHerbivoreLabel->setVisible(false);
    newCarnivoreLabel->setVisible(false);
    newPlantsLabel->setVisible(false);
    newScavengerLabel->setVisible(false);

    startButton->setVisible(false);
    stopButton->setVisible(true);
    stopButton->setText("Stop Simulation");

    speedSlider->setEnabled(true);
}

void MainWindow::resetUIForNewSimulation() {
    herbivoreInput->setVisible(true);
    carnivoreInput->setVisible(true);
    plantInput->setVisible(true);
    scavengerInput->setVisible(true);
    herbivoreLabel->setVisible(true);
    carnivoreLabel->setVisible(true);
    plantLabel->setVisible(true);
    scavengersLabel->setVisible(true);

    herbivoreInput->setEnabled(true);
    carnivoreInput->setEnabled(true);
    plantInput->setEnabled(true);
    scavengerInput->setEnabled(true);

    herbivoreCountLabel->setVisible(false);
    carnivoreCountLabel->setVisible(false);
    plantCountLabel->setVisible(false);
    scavengerCountLabel->setVisible(false);

    newHerbivoresInput->setVisible(false);
    newCarnivoresInput->setVisible(false);
    newPlantsInput->setVisible(false);
    newScavengersInput->setVisible(false);
    newHerbivoreLabel->setVisible(false);
    newCarnivoreLabel->setVisible(false);
    newPlantsLabel->setVisible(false);
    newScavengerLabel->setVisible(false);
    addOrganismsButton->setVisible(false);

    startButton->setVisible(true);
    startButton->setText("Start Simulation");
    stopButton->setVisible(false);
    showStatsButton->setVisible(false);
    restartButton->setVisible(false);
}

void MainWindow::updateUIForSimulationEnded() {
    newHerbivoresInput->setVisible(false);
    newCarnivoresInput->setVisible(false);
    newPlantsInput->setVisible(false);
    newScavengersInput->setVisible(false);
    newHerbivoreLabel->setVisible(false);
    newCarnivoreLabel->setVisible(false);
    newPlantsLabel->setVisible(false);
    newScavengerLabel->setVisible(false);
    addOrganismsButton->setVisible(false);

    herbivoreCountLabel->setVisible(true);
    carnivoreCountLabel->setVisible(true);
    plantCountLabel->setVisible(true);
    scavengerCountLabel->setVisible(true);

    startButton->setVisible(false);
    stopButton->setVisible(false);
}
void MainWindow::initializeAxisRange() {
    int maxX = 0;
    int maxY = 5;

    if (!herbivoreSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(herbivoreSeries->points().last().x()));
        maxY = std::max(maxY, static_cast<int>(herbivoreSeries->points().last().y()));
    }
    if (!carnivoreSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(carnivoreSeries->points().last().x()));
        maxY = std::max(maxY, static_cast<int>(carnivoreSeries->points().last().y()));
    }
    if (!plantSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(plantSeries->points().last().x()));
        maxY = std::max(maxY, static_cast<int>(plantSeries->points().last().y()));
    }
    if(!scavengerSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(scavengerSeries->points().last().x()));
        maxY = std::max(maxY, static_cast<int>(scavengerSeries->points().last().y()));
    }

    axisX->setRange(0, maxX);
    axisY->setRange(0, maxY);
}
void MainWindow::initializeChart() {
    // Inicjalizacja wykresów
    herbivoreSeries = new QLineSeries(this);
    carnivoreSeries = new QLineSeries(this);
    plantSeries = new QLineSeries(this);
    scavengerSeries = new QLineSeries(this);

    herbivoreSeries->setName("Herbivores");
    carnivoreSeries->setName("Carnivores");
    plantSeries->setName("Plants");
    scavengerSeries->setName("Scavengers");

    chart = new QChart();

    axisX = new QValueAxis(this);
    axisX->setLabelFormat("%i");
    axisX->setTitleText("Simulation Step");

    axisY = new QValueAxis(this);
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Number of Organisms");


    QFont axisTitleFont = axisX->titleFont();
    axisTitleFont.setPointSize(12);
    axisX->setTitleFont(axisTitleFont);
    axisY->setTitleFont(axisTitleFont);

    QFont axisLabelFont = axisX->labelsFont();
    axisLabelFont.setPointSize(10);
    axisX->setLabelsFont(axisLabelFont);
    axisY->setLabelsFont(axisLabelFont);

    // Dodanie osi do wykresu
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    // Dodanie serii do wykresu
    chart->addSeries(herbivoreSeries);
    chart->addSeries(carnivoreSeries);
    chart->addSeries(plantSeries);
    chart->addSeries(scavengerSeries);

    // Przyłączenie serii do osi
    herbivoreSeries->attachAxis(axisX);
    herbivoreSeries->attachAxis(axisY);
    carnivoreSeries->attachAxis(axisX);
    carnivoreSeries->attachAxis(axisY);
    plantSeries->attachAxis(axisX);
    plantSeries->attachAxis(axisY);
    scavengerSeries->attachAxis(axisX);
    scavengerSeries->attachAxis(axisY);

    chartView = new QChartView(chart, this);
    layout()->addWidget(chartView);

    chartView->setVisible(true);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
void MainWindow::adjustAxisRange() {
    int maxX = 0;
    int maxY = 5;

    if (!herbivoreSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(herbivoreSeries->points().last().x())) + 2;
        maxY = std::max(maxY, static_cast<int>(herbivoreSeries->points().last().y())) + 5;
    }
    if (!carnivoreSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(carnivoreSeries->points().last().x())) + 2;
        maxY = std::max(maxY, static_cast<int>(carnivoreSeries->points().last().y())) + 5;
    }
    if (!plantSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(plantSeries->points().last().x())) + 2;
        maxY = std::max(maxY, static_cast<int>(plantSeries->points().last().y())) + 5;
    }
    if(!scavengerSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(scavengerSeries->points().last().x())) + 2;
        maxY = std::max(maxY, static_cast<int>(scavengerSeries->points().last().y())) + 5;
    }

    axisX->setRange(0, maxX);
    axisY->setRange(0, maxY);
}

void MainWindow::setSimulationSpeed(int speed) {
    int interval = 1000/speed;
    timer->setInterval(interval);
}

void MainWindow::zoomIn() {
    if (view->transform().m11() < 4.0) {
        view->scale(1.2, 1.2);
    }
}
void MainWindow::zoomOut() {
    if (view->transform().m11() > 0.25) {
        view->scale(0.8, 0.8);
    }
}

MainWindow::~MainWindow() {
    if (timer) {
        timer->stop();
        delete timer;
    }
}

