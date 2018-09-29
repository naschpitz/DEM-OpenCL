#ifndef TESTSIMULATION_H
#define TESTSIMULATION_H

#include <QObject>
#include <QJsonValue>

#include "../Core/Simulation.h"

class TestSimulation : public QObject
{
    Q_OBJECT

    private:
        QJsonValue simulationJsonValue;

    public:
        TestSimulation();

    private Q_SLOTS:
        void constructor();
};

#endif // TESTSIMULATION_H
