#include "Simulation.h"

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QTime>
#include <iostream>

#include "RequestSender.h"
#include "OpenCL.h"
#include "Particle.h"

Simulation::Simulation()
{

}

Simulation::Simulation(const nlohmann::json& jsonObject)
{
    try {
        this->id = QString::fromStdString(jsonObject.at("_id").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing '_id' field in Simulation");
    }

    this->currentTime = 0;
    this->currentStep = 0;
    this->et          = 0;

    try {
        this->serverUrl = QString::fromStdString(jsonObject.at("url").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'url' field in Simulation");
    }

    try {
        this->timeStep = jsonObject.at("timeStep").get<double>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'timeStep' field in Simulation");
    }

    try {
        this->totalTime = jsonObject.at("totalTime").get<double>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'totalTime' field in Simulation");
    }

    try {
        this->frameTime = jsonObject.at("frameTime").get<double>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'frameTime' field in Simulation");
    }

    try {
        this->logTime = jsonObject.at("logTime").get<double>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'logTime' field in Simulation");
    }

    this->totalSteps = qRound(this->totalTime / this->timeStep);

    try {
        const nlohmann::json& sceneryJsonObject = jsonObject.at("scenery");
        this->scenery = Scenery(sceneryJsonObject);
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'scenery' field in Simulation");
    }

    connect(this, SIGNAL(newFrame()), &(RequestSender::getInstance()), SLOT(newFrame()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(newLog(QString)), &(RequestSender::getInstance()), SLOT(newLog(QString)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(finished()), &(RequestSender::getInstance()), SLOT(newLog()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(finished()), this, SLOT(selfDelete()));
}

Simulation::~Simulation()
{
    std::cout << "Simulation destroyed!" << "\n";
}

SimulationCL Simulation::getCL() const
{
    SimulationCL simulationCL;

    simulationCL.currentTime = this->currentTime;
    simulationCL.timeStep = this->timeStep;
    simulationCL.totalTime = this->totalTime;

    return simulationCL;
}

const QHostAddress& Simulation::getServerAddress() const
{
    return this->serverAddress;
}

void Simulation::setServerAddress(const QHostAddress& serverAddress)
{
    this->serverAddress = serverAddress;
}

const QString& Simulation::getServerUrl() const
{
    return this->serverUrl;
}

void Simulation::setServerUrl(const QString& serverUrl)
{
    this->serverUrl = serverUrl;
}

const QString& Simulation::getId() const
{
    return this->id;
}

const long& Simulation::getCurrentStep() const
{
    return this->currentStep;
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

const long& Simulation::getTotalSteps() const
{
    return this->totalSteps;
}

const double& Simulation::getStepsPerSecond() const
{
    return this->stepsPerSecond;
}

long Simulation::getEta() const
{
    long remaningSteps = this->getTotalSteps() - this->getCurrentStep();

    return remaningSteps / this->getStepsPerSecond();
}

long Simulation::getEt() const
{
    return this->et / 1000;
}

bool Simulation::isPaused() const
{
    return this->paused;
}

bool Simulation::isStopped() const
{
    return this->stoped;
}

void Simulation::run()
{
    const MaterialsManager& materialsManager = this->scenery.getMaterialsManager();

    std::vector<SimulationCL> simulationsCL = { this->getCL() };
    std::vector<SceneryCL>    sceneriesCL   = { this->scenery.getCL() };

    std::vector<ParticleCL> particlesCL = this->scenery.getObjectsManager().getParticlesCL(materialsManager).toStdVector();
    std::vector<FaceCL> facesCL = this->scenery.getObjectsManager().getFacesCL(materialsManager).toStdVector();
    std::vector<MaterialsManagerCL> materialsManagerCL = { materialsManager.getCL() };

    OpenCL::Core openClCore;

    emit this->newLog("Loading OpenCL kernel");
    openClCore.addSourceFile("../Simulation.cl");
    emit this->newLog("OpenCL kernel loaded");

    emit this->newLog("Initiating objects copy to GPU's memory");
    openClCore.writeBuffer<ParticleCL>(particlesCL);
    openClCore.writeBuffer<FaceCL>(facesCL);
    openClCore.writeBuffer<MaterialsManagerCL>(materialsManagerCL);
    openClCore.writeBuffer<SimulationCL>(simulationsCL);
    openClCore.writeBuffer<SceneryCL>(sceneriesCL);
    emit this->newLog("Objects copy to GPU's memory done");

    bool hasParticles = particlesCL.size();
    bool hasFaces = facesCL.size();

    if(!this->isPaused()) {
        if(hasParticles) {
            openClCore.addKernel("initialize_particles", particlesCL.size());
            openClCore.addArgument<ParticleCL>("initialize_particles", particlesCL);
            openClCore.addArgument<SimulationCL>("initialize_particles", simulationsCL);
        }

        if(hasFaces) {
            openClCore.addKernel("initialize_faces", facesCL.size());
            openClCore.addArgument<FaceCL>("initialize_faces", facesCL);
            openClCore.addArgument<SimulationCL>("initialize_faces", simulationsCL);
        }

        emit this->newLog("Initializing objects");
        openClCore.run();
        openClCore.clearKernels();

        openClCore.syncDevicesBuffers(particlesCL);
        openClCore.syncDevicesBuffers(facesCL);

        emit this->newLog("Objects initialized");
    }

    if(hasParticles) {
        openClCore.addKernel("reset_particles_forces", particlesCL.size());
        openClCore.addArgument<ParticleCL>("reset_particles_forces", particlesCL);
    }

    if(hasFaces) {
        openClCore.addKernel("reset_faces_forces", facesCL.size());
        openClCore.addArgument<FaceCL>("reset_faces_forces", facesCL);
    }

    if(hasParticles) {
        openClCore.addKernel("calculate_particle_to_particle", particlesCL.size());
        openClCore.addArgument<ParticleCL>("calculate_particle_to_particle", particlesCL);
        openClCore.addArgument<MaterialsManagerCL>("calculate_particle_to_particle", materialsManagerCL);
        openClCore.addArgument<SceneryCL>("calculate_particle_to_particle", sceneriesCL);
    }

    if(hasParticles && hasFaces) {
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
    }

    if(hasParticles) {
        openClCore.addKernel("apply_particles_gravity", particlesCL.size());
        openClCore.addArgument<ParticleCL>("apply_particles_gravity", particlesCL);
        openClCore.addArgument<SceneryCL>("apply_particles_gravity", sceneriesCL);
    }

    if(hasFaces) {
        openClCore.addKernel("apply_faces_gravity", facesCL.size());
        openClCore.addArgument<FaceCL>("apply_faces_gravity", facesCL);
        openClCore.addArgument<SceneryCL>("apply_faces_gravity", sceneriesCL);
    }


    if(hasParticles) {
        openClCore.addKernel("integrate_particles", particlesCL.size());
        openClCore.addArgument<ParticleCL>("integrate_particles", particlesCL);
        openClCore.addArgument<SimulationCL>("integrate_particles", simulationsCL);
    }

    if(hasFaces) {
        openClCore.addKernel("integrate_faces", facesCL.size());
        openClCore.addArgument<FaceCL>("integrate_faces", facesCL);
        openClCore.addArgument<SimulationCL>("integrate_faces", simulationsCL);
    }

    this->paused = this->stoped = false;

    uint frameSteps = this->frameTime / this->timeStep;

    QTime time;
    time.start();

    bool ran = false;
    double previousStep = this->currentStep;

    emit this->newLog("Simulation began");

    while ((this->currentStep <= this->totalSteps) && !this->paused && !this->stoped)
    {
        ran = true;
        this->currentTime = this->timeStep * this->currentStep;

        openClCore.syncDevicesBuffers(particlesCL);
        openClCore.syncDevicesBuffers(facesCL);

        if(this->currentStep % frameSteps == 0) {
            this->scenery.setParticlesCL(QVector<ParticleCL>::fromStdVector(particlesCL));
            this->scenery.setFacesCL(QVector<FaceCL>::fromStdVector(facesCL));

            emit this->newLog("New frame");
            emit this->newFrame();
        }

        int mSecElapsed = time.elapsed();

        if(mSecElapsed > (this->logTime * 1000) || this->currentStep == this->totalSteps) {
            long numSteps = this->currentStep - previousStep;
            this->stepsPerSecond = ((double)numSteps / mSecElapsed) * 1000;
            this->et += mSecElapsed;

            previousStep = this->currentStep;
            time.restart();

            emit this->newLog("New log message");
        }

        openClCore.run();

        this->currentStep++;
    }

    // Decrement because the last loop dosen't happen.
    if(ran && !this->isPaused())
        this->currentStep--;

    if(this->isPaused()) {
        openClCore.syncDevicesBuffers(particlesCL);
        openClCore.syncDevicesBuffers(facesCL);

        this->scenery.setParticlesCL(QVector<ParticleCL>::fromStdVector(particlesCL));
        this->scenery.setFacesCL(QVector<FaceCL>::fromStdVector(facesCL));

        this->newLog("Simulation paused");
    }

    if(this->isStopped())
        this->newLog("Simulation stopped");

    emit this->newLog("Simulation ended");
}

void Simulation::pause()
{
    this->paused = true;
}

void Simulation::stop()
{
    this->stoped = true;

    // TODO: Correct error when stopping a paused simulation, deadlock involved.
    /*
    if(this->isPaused())
        emit this->newLog();
    */
}

void Simulation::selfDelete()
{
    if(this->isStopped() || !this->isPaused()) {
        this->deleteLater();
    }
}
