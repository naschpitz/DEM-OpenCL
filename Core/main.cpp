#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <iostream>

#include "Simulation.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QFile simulationFile("../SimulationRun.json");
    simulationFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QString simulationFileString = simulationFile.readAll();
    simulationFile.close();

    QJsonDocument simulatioJsonDocument = QJsonDocument::fromJson(simulationFileString.toUtf8());
    QJsonValue simulationJsonValue = simulatioJsonDocument.object();

    Simulation simulation(simulationJsonValue);

    simulation.run();

    exit(0);

    return a.exec();
}
