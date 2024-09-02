#ifndef ECOSYSTEM_SIMULATIONTHREAD_H
#define ECOSYSTEM_SIMULATIONTHREAD_H
#include <QThread>
#include <QMutex>
#include "Reserve.h"

class SimulationThread : public QThread {
Q_OBJECT

public:
    SimulationThread(Reserve* reserve, QObject* parent = nullptr)
            : QThread(parent), reserve(reserve), stopRequested(false) {}

    void requestStop();


protected:
    void run() override;

signals:
    void simulationStepCompleted();

private:
    Reserve* reserve;
    QMutex mutex;  // Mutex do synchronizacji dostępu do zmiennych wątku
    bool stopRequested;  // Flaga sygnalizująca zakończenie symulacji
};

#endif //ECOSYSTEM_SIMULATIONTHREAD_H
