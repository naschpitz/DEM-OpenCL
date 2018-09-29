#include "TestSimulation.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QTest>
#include <math.h>

TestSimulation::TestSimulation()
{
    QFile simulationFile("../Simulation.json");
    simulationFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QString simulationFileString = simulationFile.readAll();
    simulationFile.close();

    QJsonDocument simulationJsonDocument = QJsonDocument::fromJson(simulationFileString.toUtf8());
    this->simulationJsonValue = simulationJsonDocument.object();
}

void TestSimulation::constructor()
{
    Simulation simulation(this->simulationJsonValue);

    Vector3D expectedGravity(0, 0, -9.81);
    double expectedTimeStep  = 0.00001;
    double expectedTotalTime = 10;

    QCOMPARE(simulation.getGravity(), expectedGravity);
    QCOMPARE(simulation.getTimeStep(), expectedTimeStep);
    QCOMPARE(simulation.getTotalTime(), expectedTotalTime);

    QVERIFY(simulation.getScenery().getMaterialsManager().getMaterials().count() == 3);
    QVERIFY(simulation.getScenery().getObjectsManager().getNonSolidObjects().count() == 2);
}

