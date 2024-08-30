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
   // QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout *leftLayout = new QVBoxLayout();

    // Elementy UI
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

    leftLayout->addWidget(herbivoreLabel);
    leftLayout->addWidget(herbivoreInput);
    leftLayout->addWidget(carnivoreLabel);
    leftLayout->addWidget(carnivoreInput);
    leftLayout->addWidget(plantLabel);
    leftLayout->addWidget(plantInput);

    herbivoreCountLabel = new QLabel("Herbivores: 0", this);
    carnivoreCountLabel = new QLabel("Carnivores: 0", this);
    plantCountLabel = new QLabel("Plants: 0", this);

    leftLayout->addWidget(herbivoreCountLabel);
    leftLayout->addWidget(carnivoreCountLabel);
    leftLayout->addWidget(plantCountLabel);
    herbivoreCountLabel->setVisible(false);
    carnivoreCountLabel->setVisible(false);
    plantCountLabel->setVisible(false);

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

    newHerbivoreLabel = new QLabel("New Herbivores:", this);
    newCarnivoreLabel = new QLabel("New Carnivores:", this);
    newPlantsLabel = new QLabel("New Plants:", this);

    addOrganismsButton = new QPushButton("Add Organisms", this);

    leftLayout->addWidget(newHerbivoreLabel);
    leftLayout->addWidget(newHerbivoresInput);
    leftLayout->addWidget(newCarnivoreLabel);
    leftLayout->addWidget(newCarnivoresInput);
    leftLayout->addWidget(newPlantsLabel);
    leftLayout->addWidget(newPlantsInput);
    leftLayout->addWidget(addOrganismsButton);
    newHerbivoresInput->setVisible(false);
    newCarnivoresInput->setVisible(false);
    newPlantsInput->setVisible(false);
    newHerbivoreLabel->setVisible(false);
    newCarnivoreLabel->setVisible(false);
    newPlantsLabel->setVisible(false);
    addOrganismsButton->setVisible(false);

    // scena, widok
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    scene->setBackgroundBrush(Qt::darkGreen);
    leftLayout->addWidget(view);

    initializeChart();

    mainLayout->addLayout(leftLayout, 2);
    mainLayout->addWidget(chartView, 1);

    setCentralWidget(centralWidget);

    initializeSimulationController();

    initializeAxisRange();

    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartSimulation);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopSimulation);
    connect(showStatsButton, &QPushButton::clicked, this, &MainWindow::onShowStatsButtonClicked);
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::onRestartButtonClicked);
    connect(addOrganismsButton, &QPushButton::clicked, this, &MainWindow::onAddOrganismsClicked);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onSimulationStepCompleted);

    if (auto reserve = simulationController->getReserve()) {
        connect(reserve.get(), &Reserve::simulationEnded, this, &MainWindow::onSimulationEnded);
    }


    scene->setSceneRect(0, 0, 1000, 800);
}
void MainWindow::initializeSimulationController() {
    int numHerbivores = herbivoreInput->value();
    int numCarnivores = carnivoreInput->value();
    int numPlants = plantInput->value();

    simulationController = std::make_unique<SimulationController>(800, 700, numHerbivores, numCarnivores, numPlants);
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
    if (timer->isActive()) {
        timer->stop();
    }

    int numHerbivores = herbivoreInput->value();
    int numCarnivores = carnivoreInput->value();
    int numPlants = plantInput->value();


    if (isRestarting) {
        isRestarting = false;
        isPaused = false;

        simulationController = std::make_unique<SimulationController>(700, 450, numHerbivores, numCarnivores, numPlants);
        simulationTimer.restart();
        updateUIForSimulationRunning();

        timer->start(1000);
        updateScene();
        updateCounts();
        return;
    }

    if (isPaused) {
        isPaused = false;
        updateUIForSimulationRunning();
        timer->start(1000);
        return;
    }

    simulationController = std::make_unique<SimulationController>(700, 450, numHerbivores, numCarnivores, numPlants);
    simulationTimer.restart();
    updateUIForSimulationRunning();
    timer->start(1000);
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

    herbivoreInput->setEnabled(true);
    carnivoreInput->setEnabled(true);
    plantInput->setEnabled(true);

    resetUIForNewSimulation();

    herbivoreSeries->clear();
    carnivoreSeries->clear();
    plantSeries->clear();


    int numHerbivores = herbivoreInput->value();
    int numCarnivores = carnivoreInput->value();
    int numPlants = plantInput->value();

    simulationController = std::make_unique<SimulationController>(700, 450, numHerbivores, numCarnivores, numPlants);


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
        addOrganismsButton->setVisible(true);

        newHerbivoreLabel->setVisible(true);
        newCarnivoreLabel->setVisible(true);
        newPlantsLabel->setVisible(true);
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
    QMessageBox::information(this, "Simulation Ended", "All herbivores and carnivores are dead. The simulation has ended.");

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
        herbivoreSeries->append(step, simulationController->getReserve()->getHerbivores().size());
        carnivoreSeries->append(step, simulationController->getReserve()->getCarnivores().size());
        plantSeries->append(step, simulationController->getReserve()->getPlants().size());
    }
    adjustAxisRange();
    chart->update();

    if (simulationController->getReserve()->getHerbivores().empty() &&
        simulationController->getReserve()->getCarnivores().empty()) {
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

    static QPixmap plantPixmap(":/gui/img/flower.png");
    static QPixmap poisonousPlantPixmap(":/gui/img/poisonous.png");
    static QPixmap herbivorePixmap(":/gui/img/deer.png");
    static QPixmap carnivorePixmap(":/gui/img/wolf.png");

    int organismSize = 30;

    // Usunięcie martwych organizmów
    for (auto it = organismItems.begin(); it != organismItems.end();) {
        if (it->first && !it->first->isAlive()) {
            scene->removeItem(it->second);
            delete it->second;
            it = organismItems.erase(it);
        } else {
            ++it;
        }
    }

    auto updateOrAddOrganism = [&](auto& organisms, QPixmap& pixmap) {
        for (const auto& organism : organisms) {
            if (organism && organism->isAlive()) {
                auto it = organismItems.find(organism.get());
                if (it != organismItems.end()) {
                    // Aktualizacja położenia
                    if (it->second->pos() != QPointF(organism->getX(), organism->getY())) {
                        it->second->setPos(organism->getX(), organism->getY());
                    }
                } else {
                    // Dodanie organizmów
                    auto organismItem = new InteractiveOrganism(pixmap, organism.get(), organismSize, organismSize);
                    organismItems[organism.get()] = organismItem;
                    scene->addItem(organismItem);
                }
            }
        }
    };

    updateOrAddOrganism(reserve->getHerbivores(), herbivorePixmap);
    updateOrAddOrganism(reserve->getCarnivores(), carnivorePixmap);

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
            "Herbivores poisoned by plants: %7\n"
            "Average lifespan of herbivores: %8\n"
            "Average lifespan of carnivores: %9\n"
            "Average energy of herbivores: %10\n"
            "Average energy of carnivores: %11")
            .arg(stats.birthsHerbivores)
            .arg(stats.birthsCarnivores)
            .arg(stats.deathsHerbivores)
            .arg(stats.deathsCarnivores)
            .arg(stats.plantsEaten)
            .arg(stats.herbivoresEaten)
            .arg(stats.herbivoresPoisoned)
            .arg(stats.averageLifeSpanHerbivores)
            .arg(stats.averageLifeSpanCarnivores)
            .arg(stats.averageEnergyHerbivores)
            .arg(stats.averageEnergyCarnivores);

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

    startButton->setVisible(false);
    stopButton->setVisible(true);
    stopButton->setText("Stop Simulation");
}

void MainWindow::resetUIForNewSimulation() {
    herbivoreInput->setVisible(true);
    carnivoreInput->setVisible(true);
    plantInput->setVisible(true);
    herbivoreLabel->setVisible(true);
    carnivoreLabel->setVisible(true);
    plantLabel->setVisible(true);

    herbivoreInput->setEnabled(true);
    carnivoreInput->setEnabled(true);
    plantInput->setEnabled(true);

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
    startButton->setText("Start Simulation");
    stopButton->setVisible(false);
    showStatsButton->setVisible(false);
    restartButton->setVisible(false);
}

void MainWindow::updateUIForSimulationEnded() {
    newHerbivoresInput->setVisible(false);
    newCarnivoresInput->setVisible(false);
    newPlantsInput->setVisible(false);
    newHerbivoreLabel->setVisible(false);
    newCarnivoreLabel->setVisible(false);
    newPlantsLabel->setVisible(false);
    addOrganismsButton->setVisible(false);

    herbivoreCountLabel->setVisible(true);
    carnivoreCountLabel->setVisible(true);
    plantCountLabel->setVisible(true);

    startButton->setVisible(false);
    stopButton->setVisible(false);
}
void MainWindow::initializeAxisRange() {
    int maxX = 5; // Start with a reasonable default range
    int maxY = 5; // Start with a reasonable default range

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

    axisX->setRange(0, maxX);
    axisY->setRange(0, maxY);
}
void MainWindow::initializeChart() {
    // Inicjalizacja wykresów
    herbivoreSeries = new QLineSeries(this);
    carnivoreSeries = new QLineSeries(this);
    plantSeries = new QLineSeries(this);

    herbivoreSeries->setName("Herbivores");
    carnivoreSeries->setName("Carnivores");
    plantSeries->setName("Plants");

    chart = new QChart();

    axisX = new QValueAxis(this);
    axisX->setLabelFormat("%i");
    axisX->setTitleText("Simulation Step");

    axisY = new QValueAxis(this);
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Number of Organisms");


    QFont axisTitleFont = axisX->titleFont();
    axisTitleFont.setPointSize(12); // Set font size for titles
    axisX->setTitleFont(axisTitleFont);
    axisY->setTitleFont(axisTitleFont);

    // Set font for axis labels
    QFont axisLabelFont = axisX->labelsFont();
    axisLabelFont.setPointSize(10); // Set font size for labels
    axisX->setLabelsFont(axisLabelFont);
    axisY->setLabelsFont(axisLabelFont);



    // Dodanie osi do wykresu
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    // Dodanie serii do wykresu
    chart->addSeries(herbivoreSeries);
    chart->addSeries(carnivoreSeries);
    chart->addSeries(plantSeries);

    // Przyłączenie serii do osi
    herbivoreSeries->attachAxis(axisX);
    herbivoreSeries->attachAxis(axisY);
    carnivoreSeries->attachAxis(axisX);
    carnivoreSeries->attachAxis(axisY);
    plantSeries->attachAxis(axisX);
    plantSeries->attachAxis(axisY);

    chartView = new QChartView(chart, this);
    layout()->addWidget(chartView);

    chartView->setVisible(true); // Make chart view visible from the beginning
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
void MainWindow::adjustAxisRange() {
    // Calculate the dynamic ranges for the axes based on the current data
    int maxX = 5;
    int maxY = 5;

    if (!herbivoreSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(herbivoreSeries->points().last().x())) + 5;
        maxY = std::max(maxY, static_cast<int>(herbivoreSeries->points().last().y())) + 5;
    }
    if (!carnivoreSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(carnivoreSeries->points().last().x())) + 5;
        maxY = std::max(maxY, static_cast<int>(carnivoreSeries->points().last().y())) + 5;
    }
    if (!plantSeries->points().isEmpty()) {
        maxX = std::max(maxX, static_cast<int>(plantSeries->points().last().x())) + 5;
        maxY = std::max(maxY, static_cast<int>(plantSeries->points().last().y())) + 5;
    }

    axisX->setRange(0, maxX);
    axisY->setRange(0, maxY);
}
MainWindow::~MainWindow() {
    timer->stop();
    delete timer;
}

