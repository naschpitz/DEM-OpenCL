#include "Simulation.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <iostream>

#include "RequestSender.h"
#include "OpenCL.h"
#include "Particle.h"

Simulation::Simulation(const QJsonObject& jsonObject)
{
    this->id = jsonObject["_id"].toString();

    this->currentTime = 0;
    this->currentStep = 0;
    this->frameTime   = jsonObject["frameTime"].toDouble();
    this->timeStep    = jsonObject["timeStep"].toDouble();
    this->totalTime   = jsonObject["totalTime"].toDouble();
    this->totalSteps  = this->totalTime / this->timeStep;

    QJsonObject sceneryJsonObject = jsonObject["scenery"].toObject();
    this->scenery = Scenery(sceneryJsonObject);

    connect(this, &Simulation::newFrame, &RequestSender::newFrame);
}

const QString& Simulation::getId() const
{
    return this->id;
}

void Simulation::addFrame()
{
    QJsonObject newFrame;

    newFrame["currentTime"] = this->currentTime;
    newFrame["currentStep"] = (int)this->currentStep;
    newFrame["scenery"] = this->scenery.getJson();

    emit this->newFrame(newFrame);
}

SimulationCL Simulation::getCL() const
{
    SimulationCL simulationCL;

    simulationCL.currentTime = this->currentTime;
    simulationCL.timeStep = this->timeStep;
    simulationCL.totalTime = this->totalTime;

    return simulationCL;
}

const double& Simulation::getCurrentTime() const
{
    return this->currentTime;
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

bool Simulation::isPaused() const
{
    return this->paused;
}

bool Simulation::isStoped() const
{
    return this->stoped;
}

void Simulation::run()
{
    this->paused = this->stoped = false;

    OpenCL::Core openClCore;

    openClCore.addSourceFile("../Simulation.cl");

    const MaterialsManager& materialsManager = this->scenery.getMaterialsManager();

    std::vector<SimulationCL> simulationsCL = { this->getCL() };
    std::vector<SceneryCL>    sceneriesCL   = { this->scenery.getCL() };

    std::vector<ParticleCL> particlesCL = this->scenery.getObjectsManager().getParticlesCL(materialsManager).toStdVector();
    std::vector<FaceCL> facesCL = this->scenery.getObjectsManager().getFacesCL(materialsManager).toStdVector();
    std::vector<MaterialsManagerCL> materialsManagerCL = { materialsManager.getCL() };

    openClCore.writeBuffer<ParticleCL>(particlesCL);
    openClCore.writeBuffer<FaceCL>(facesCL);
    openClCore.writeBuffer<MaterialsManagerCL>(materialsManagerCL);
    openClCore.writeBuffer<SimulationCL>(simulationsCL);
    openClCore.writeBuffer<SceneryCL>(sceneriesCL);

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
    openClCore.addArgument<SceneryCL>("calculate_particle_to_particle", sceneriesCL);

    openClCore.addKernel("calculate_particle_to_face", particlesCL.size());
    openClCore.addArgument<ParticleCL>("calculate_particle_to_face", particlesCL);
    openClCore.addArgument<FaceCL>("calculate_particle_to_face", facesCL);
    openClCore.addArgument<MaterialsManagerCL>("calculate_particle_to_face", materialsManagerCL);
    openClCore.addArgument<SceneryCL>("calculate_particle_to_face", sceneriesCL);

    openClCore.addKernel("calculate_face_to_particle", facesCL.size());
    openClCore.addArgument<FaceCL>("calculate_face_to_particle", facesCL);
    openClCore.addArgument<ParticleCL>("calculate_face_to_particle", particlesCL);
    openClCore.addArgument<MaterialsManagerCL>("calculate_face_to_particle", materialsManagerCL);
    openClCore.addArgument<SceneryCL>("calculate_face_to_particle", sceneriesCL);

    openClCore.addKernel("apply_particles_gravity", particlesCL.size());
    openClCore.addArgument<ParticleCL>("apply_particles_gravity", particlesCL);
    openClCore.addArgument<SceneryCL>("apply_particles_gravity", sceneriesCL);

    openClCore.addKernel("apply_faces_gravity", facesCL.size());
    openClCore.addArgument<FaceCL>("apply_faces_gravity", facesCL);
    openClCore.addArgument<SceneryCL>("apply_faces_gravity", sceneriesCL);

    openClCore.addKernel("integrate_particles", particlesCL.size());
    openClCore.addArgument<ParticleCL>("integrate_particles", particlesCL);
    openClCore.addArgument<SimulationCL>("integrate_particles", simulationsCL);

    openClCore.addKernel("integrate_faces", facesCL.size());
    openClCore.addArgument<FaceCL>("integrate_faces", facesCL);
    openClCore.addArgument<SimulationCL>("integrate_faces", simulationsCL);

    uint frameSteps = this->frameTime / this->timeStep;

    while((this->currentTime < this->totalTime) && !this->paused && !this->stoped)
    {        
        if (this->currentStep % frameSteps == 0) {
            openClCore.readBuffer(particlesCL);
            openClCore.readBuffer(facesCL);

            this->scenery.setParticlesCL(QVector<ParticleCL>::fromStdVector(particlesCL));
            this->scenery.setFacesCL(QVector<FaceCL>::fromStdVector(facesCL));

            this->addFrame();
        }

        openClCore.run();

        this->currentTime += this->timeStep;
        this->currentStep += 1;
    }

    if(this->paused || this->stoped) {
        openClCore.readBuffer(particlesCL);
        openClCore.readBuffer(facesCL);

        this->scenery.setParticlesCL(QVector<ParticleCL>::fromStdVector(particlesCL));
        this->scenery.setFacesCL(QVector<FaceCL>::fromStdVector(facesCL));

        this->addFrame();
    }
}

void Simulation::pause()
{
    this->paused = true;
}

void Simulation::stop()
{
    this->stoped = true;
}


