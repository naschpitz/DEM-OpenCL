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
    std::vector<FaceCL> facesCL = this->scenery.getObjectsManager().getFacesCL(materialsManager).toStdVector();
    std::vector<MaterialsManagerCL> materialsManagerCL = { materialsManager.getCL() };

    SimulationCL simulationCL = this->getCL();
    simulationCL.numParticles = particlesCL.size();
    simulationCL.numFaces = facesCL.size();

    std::vector<SimulationCL> simulationsCL = { simulationCL };

    openClCore.writeBuffer<ParticleCL>(particlesCL);
    openClCore.writeBuffer<FaceCL>(facesCL);
    openClCore.writeBuffer<MaterialsManagerCL>(materialsManagerCL);
    openClCore.writeBuffer<SimulationCL>(simulationsCL);

    openClCore.addKernel("initialize_particles", particlesCL.size());
    openClCore.addArgument<ParticleCL>("initialize_particles", particlesCL);
    openClCore.addArgument<SimulationCL>("initialize_particles", simulationsCL);

    openClCore.addKernel("initialize_faces", facesCL.size());
    openClCore.addArgument<FaceCL>("initialize_faces", facesCL);
    openClCore.addArgument<SimulationCL>("initialize_faces", simulationsCL);

    openClCore.run();
    openClCore.clearKernels();

    openClCore.addKernel("calculate_particle_to_particle", particlesCL.size());
    openClCore.addArgument<ParticleCL>("calculate_particle_to_particle", particlesCL);
    openClCore.addArgument<MaterialsManagerCL>("calculate_particle_to_particle", materialsManagerCL);
    openClCore.addArgument<SimulationCL>("calculate_particle_to_particle", simulationsCL);

    openClCore.addKernel("calculate_particle_to_face", particlesCL.size());
    openClCore.addArgument<ParticleCL>("calculate_particle_to_face", particlesCL);
    openClCore.addArgument<FaceCL>("calculate_particle_to_face", facesCL);
    openClCore.addArgument<MaterialsManagerCL>("calculate_particle_to_face", materialsManagerCL);
    openClCore.addArgument<SimulationCL>("calculate_particle_to_face", simulationsCL);

    openClCore.addKernel("calculate_face_to_particle", facesCL.size());
    openClCore.addArgument<FaceCL>("calculate_face_to_particle", facesCL);
    openClCore.addArgument<ParticleCL>("calculate_face_to_particle", particlesCL);
    openClCore.addArgument<MaterialsManagerCL>("calculate_face_to_particle", materialsManagerCL);
    openClCore.addArgument<SimulationCL>("calculate_face_to_particle", simulationsCL);

    openClCore.addKernel("apply_particles_gravity", particlesCL.size());
    openClCore.addArgument<ParticleCL>("apply_particles_gravity", particlesCL);
    openClCore.addArgument<SimulationCL>("apply_particles_gravity", simulationsCL);

    openClCore.addKernel("apply_faces_gravity", facesCL.size());
    openClCore.addArgument<FaceCL>("apply_faces_gravity", facesCL);
    openClCore.addArgument<SimulationCL>("apply_faces_gravity", simulationsCL);

    openClCore.addKernel("integrate_particles", particlesCL.size());
    openClCore.addArgument<ParticleCL>("integrate_particles", particlesCL);
    openClCore.addArgument<SimulationCL>("integrate_particles", simulationsCL);

    openClCore.addKernel("integrate_faces", facesCL.size());
    openClCore.addArgument<FaceCL>("integrate_faces", facesCL);
    openClCore.addArgument<SimulationCL>("integrate_faces", simulationsCL);

    uint logSteps = this->logTime / this->timeStep;

    while(this->currentTime < this->totalTime)
    {        
        if (this->currentStep % logSteps == 0) {
            openClCore.readBuffer(particlesCL);
            openClCore.readBuffer(facesCL);

            this->scenery.setParticlesCL(QVector<ParticleCL>::fromStdVector(particlesCL));
            this->scenery.setFacesCL(QVector<FaceCL>::fromStdVector(facesCL));

            this->addFrame();
            this->writeLog();
        }

        openClCore.run();

        this->currentTime += this->timeStep;
        this->currentStep += 1;
    }
}
