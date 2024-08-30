#include "SimulationThread.h"

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
            reserve->simulateStep();
        }


        emit simulationStepCompleted();
        QThread::msleep(100);
    }
}


