#ifndef ECOSYSTEM_MAINWINDOW_H
#define ECOSYSTEM_MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSpinBox>
#include <QMutex>
#include <unordered_map>
#include "SimulationController.h"
#include <QElapsedTimer>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE


class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartSimulation();
    void onStopSimulation();
    void onSimulationEnded();
    void onShowStatsButtonClicked();
    void onRestartButtonClicked();
    void onAddOrganismsClicked();
    void onSimulationStepCompleted();
    void setSimulationSpeed(int speed);

private:
    void updateScene();
    void updateCounts();
    void keyPressEvent(QKeyEvent *event);
    void initializeSimulationController();

    void updateUIForSimulationRunning();
    void resetUIForNewSimulation();
    void updateUIForSimulationEnded();
    void initializeAxisRange();
    void initializeChart();
    void adjustAxisRange();

    bool isPaused;
    bool simulationEnded;
    bool isRestarting;


    std::unique_ptr<SimulationController> simulationController;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QMutex mutex;
    std::unordered_map<Organism*, QGraphicsPixmapItem*> organismItems;

    QLabel *herbivoreCountLabel;
    QLabel *carnivoreCountLabel;
    QLabel *plantCountLabel;
    QLabel *scavengerCountLabel;
    QLabel *newPlantsLabel;
    QLabel *newHerbivoreLabel;
    QLabel *newCarnivoreLabel;
    QLabel *newScavengerLabel;

    QSpinBox *herbivoreInput;
    QSpinBox *carnivoreInput;
    QSpinBox *plantInput;
    QSpinBox *scavengerInput;
    QSpinBox* newHerbivoresInput;
    QSpinBox* newCarnivoresInput;
    QSpinBox* newPlantsInput;
    QSpinBox* newScavengersInput;

    QLabel *herbivoreLabel;
    QLabel *carnivoreLabel;
    QLabel *plantLabel;
    QLabel *scavengersLabel;
    QLabel *speedLabel;

    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *showStatsButton;
    QPushButton *restartButton;
    QPushButton* addOrganismsButton;

    QTimer *timer;
    QElapsedTimer simulationTimer;
    qint64 simulationDuration;
    QSlider *speedSlider;

    QtCharts::QLineSeries *herbivoreSeries;
    QtCharts::QLineSeries *carnivoreSeries;
    QtCharts::QLineSeries *plantSeries;
    QtCharts::QLineSeries *scavengerSeries;

    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;

    QChartView* chartView;
};

#endif //ECOSYSTEM_MAINWINDOW_H
