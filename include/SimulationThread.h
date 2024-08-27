#ifndef ECOSYSTEM_SIMULATIONTHREAD_H
#define ECOSYSTEM_SIMULATIONTHREAD_H
#include <QThread>
#include <QMutex>
#include "Reserve.h"

class SimulationThread : public QThread {
Q_OBJECT  // Makro Q_OBJECT jest wymagane do użycia sygnałów i slotów

public:
    SimulationThread(Reserve* reserve, QObject* parent = nullptr)
            : QThread(parent), reserve(reserve), stopRequested(false) {}  // Konstruktor

    void requestStop();


protected:
    void run() override;

signals:
    void simulationStepCompleted();  // Sygnał emitowany po zakończeniu każdego kroku symulacji

private:
    Reserve* reserve;  // Wskaźnik na obiekt rezerwy, który symulujemy
    QMutex mutex;  // Mutex do synchronizacji dostępu do zmiennych wątku
    bool stopRequested;  // Flaga sygnalizująca zakończenie symulacji
};

#endif //ECOSYSTEM_SIMULATIONTHREAD_H
