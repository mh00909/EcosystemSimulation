#include "SimulationThread.h"

// Konstruktor i inne metody są już zdefiniowane w SimulationThread.h
// Jeżeli chcesz mieć oddzielną implementację, przenieś funkcje tutaj



void SimulationThread::requestStop() {
    QMutexLocker locker(&mutex);
    stopRequested = true;
}

void SimulationThread::run() {
    while (true) {
        {
            QMutexLocker locker(&mutex);
            if (stopRequested) {
                break;
            }
        }

        {
            QMutexLocker locker(&mutex); // Upewnij się, że dane są bezpieczne w kontekście wielu wątków
            reserve->simulateStep();
        }

        emit simulationStepCompleted();
        QThread::msleep(100);
    }
}


