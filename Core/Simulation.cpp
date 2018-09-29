#include "Simulation.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <iostream>

#include "OpenCL.h"
#include "Particle.h"

Simulation::Simulation(const QJsonValue& jsonValue)
{
    this->currentTime = 0;
    this->currentStep = 0;
    this->timeStep   = jsonValue["timeStep"].toDouble();
    this->totalTime  = jsonValue["totalTime"].toDouble();
    this->logTime    = jsonValue["logTime"].toDouble();
    this->totalSteps = this->totalTime / this->timeStep;

    QJsonArray gravityForceArray = jsonValue["gravity"].toArray();
    this->gravity = Vector3D(gravityForceArray[0].toDouble(), gravityForceArray[1].toDouble(), gravityForceArray[2].toDouble());

    QJsonValue sceneryJsonValue = jsonValue["scenery"];
    this->scenery = Scenery(sceneryJsonValue);
}

void Simulation::addFrame()
{
    this->log = QJsonObject();

    QJsonObject newFrame;
    newFrame["currentTime"] = this->currentTime;
    newFrame["totalTime"] = this->totalTime;
    newFrame["currentStep"] = (int)this->currentStep;
    newFrame["totalSteps"] = (int)this->totalSteps;

    newFrame["scenery"] = this->scenery.getJson();

    this->log["simulation-frame"] = newFrame;
}

QJsonObject Simulation::getJson() const
{
    return this->log;
}

void Simulation::writeLog() const
{
    QFile file(QString("Simulation-Result-%1.json").arg(this->currentStep));
    file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);

    QTextStream stream(&file);
    QByteArray log = QJsonDocument(this->getJson()).toJson();
    stream << log;

    file.flush();
    file.close();
}

SimulationCL Simulation::getCL() const
{
    SimulationCL simulationCL;

    simulationCL.currentTime = this->currentTime;
    simulationCL.timeStep = this->timeStep;
    simulationCL.totalTime = this->totalTime;
    simulationCL.gravity = {this->gravity.getX(), this->gravity.getY(), this->gravity.getZ(), 0};

    return simulationCL;
}

const double& Simulation::getCurrentTime() const
{
    return this->currentTime;
}

const Vector3D& Simulation::getGravity() const
{
    return this->gravity;
}

const Scenery& Simulation::getScenery() const
{
    return this->scenery;
}

const double& Simulation::getTimeStep() const
{
    return this->timeStep;
}

const double& Simulation::getTotalTime() const
{
    return this->totalTime;
}

void Simulation::run()
{
    OpenCL::Core openClCore;

    openClCore.addSourceFile("../Simulation.cl");

    const MaterialsManager& materialsManager = this->scenery.getMaterialsManager();

    std::vector<ParticleCL> particlesCL = this->scenery.getObjectsManager().getParticlesCL(materialsManager).toStdVector();
    std::vector<MaterialsManagerCL> materialsManagerCL = { materialsManager.getCL() };
    std::vector<SimulationCL> simulationCL = { this->getCL() };

    openClCore.writeBuffer<ParticleCL>(particlesCL);
    openClCore.writeBuffer<MaterialsManagerCL>(materialsManagerCL);
    openClCore.writeBuffer<SimulationCL>(simulationCL);

    openClCore.addKernel("initialize", particlesCL.size());
    openClCore.addArgument<ParticleCL>("initialize", particlesCL);
    openClCore.addArgument<SimulationCL>("initialize", simulationCL);

    openClCore.run();
    openClCore.clearKernels();

    openClCore.addKernel("calculate_forces", particlesCL.size());
    openClCore.addArgument<ParticleCL>("calculate_forces", particlesCL);
    openClCore.addArgument<MaterialsManagerCL>("calculate_forces", materialsManagerCL);
    openClCore.addArgument<SimulationCL>("calculate_forces", simulationCL);

    openClCore.addKernel("integrate", particlesCL.size());
    openClCore.addArgument<ParticleCL>("integrate", particlesCL);
    openClCore.addArgument<SimulationCL>("integrate", simulationCL);

    uint logSteps = this->logTime / this->timeStep;

    while(this->currentTime < this->totalTime)
    {        
        if (this->currentStep % logSteps == 0) {
            openClCore.readBuffer(particlesCL);
            this->scenery.setParticlesCL(QVector<ParticleCL>::fromStdVector(particlesCL));

            this->addFrame();
            this->writeLog();
        }

        openClCore.run();

        this->currentTime += this->timeStep;
        this->currentStep += 1;
    }
}
