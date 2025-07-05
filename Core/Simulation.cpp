#include "Simulation.h"
#include "SimulationLogger.h"

#include <QDateTime>
#include <QFile>
#include <QMutexLocker>
#include <QString>
#include <QTextStream>
#include <iostream>

#include <OpenCLWrapper/OCLW_Core.hpp>

#include "RequestSender.h"
#include "Particle.h"

Simulation::Simulation()
{
    this->initialized = false;
    this->paused = false;
    this->stoped = false;
    this->logger = nullptr;
}

Simulation::Simulation(const nlohmann::json& jsonObject)
{
    this->initialized = false;
    this->paused = false;
    this->stoped = false;
    this->logger = nullptr;

    try {
        this->id = QString::fromStdString(jsonObject.at("_id").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing '_id' field in Simulation");
    }

    try {
        this->instance = QString::fromStdString(jsonObject.at("instance").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'instance' field in Simulation");
    }

    try {
        this->primary = jsonObject.at("primary").get<bool>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'primary' field in Simulation");
    }

    this->currentTime    = 0;
    this->currentStep    = 0;
    this->stepsPerSecond = 0;
    this->et             = 0;

    try {
        this->interfaceUrl = QString::fromStdString(jsonObject.at("url").get<std::string>());
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

    try {
        this->detailedFramesDiv = jsonObject.at("detailedFramesDiv").get<long>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'detailedFramesDiv' field in Simulation");
    }

    try {
        this->calcNeighTimeInt = jsonObject.at("calcNeighTimeInt").get<double>();
        this->calcNeighStepsInt = qRound(this->calcNeighTimeInt / this->timeStep);
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'calcNeighTimeInt' field in Simulation");
    }

    try {
        this->neighDistThresMult = jsonObject.at("neighDistThresMult").get<double>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'neighDistThresMult' field in Simulation");
    }

    try {
        this->multiGPU = jsonObject.at("multiGPU").get<bool>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'multiGPU' field in Simulation");
    }

    this->totalSteps = qRound(this->totalTime / this->timeStep);

    try {
        const nlohmann::json& sceneryJsonObject = jsonObject.at("scenery");
        this->scenery = Scenery(sceneryJsonObject);
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'scenery' field in Simulation");
    }

    connect(this, SIGNAL(newLog(QString)), &(RequestSender::getInstance()), SLOT(newLog(QString)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(newFrame(bool)), &(RequestSender::getInstance()), SLOT(newFrame(bool)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(finished()), this, SLOT(selfDelete()));

    this->logger = new SimulationLogger(this, this);
}

Simulation::~Simulation()
{
    if (this->logger) {
        this->logger->stop();
        delete this->logger;
        this->logger = nullptr;
    }

    std::cout << "Simulation destroyed!" << std::endl;
    std::cout.flush();
}

void Simulation::initialize()
{
    this->scenery.initialize();
}

SimulationCL Simulation::getCL() const
{
    QMutexLocker locker(&this->dataMutex);

    SimulationCL simulationCL;

    simulationCL.currentTime = this->currentTime;
    simulationCL.currentStep = this->currentStep;

    simulationCL.timeStep = this->timeStep;
    simulationCL.totalTime = this->totalTime;

    simulationCL.calcNeighStepsInt = this->calcNeighStepsInt;
    simulationCL.neighDistThresMult = this->neighDistThresMult;

    return simulationCL;
}

const QHostAddress& Simulation::getInterfaceAddress() const
{
    return this->interfaceAddress;
}

void Simulation::setInterfaceAddress(const QHostAddress& serverAddress)
{
    this->interfaceAddress = serverAddress;
}

const QString& Simulation::getInterfaceUrl() const
{
    return this->interfaceUrl;
}

void Simulation::setInterfaceUrl(const QString& serverUrl)
{
    this->interfaceUrl = serverUrl;
}

const QString& Simulation::getId() const
{
    // No mutex needed - id is constant throughout simulation lifecycle
    return this->id;
}

const QString& Simulation::getInstance() const
{
    QMutexLocker locker(&this->dataMutex);
    return this->instance;
}

const ulong& Simulation::getCurrentStep() const
{
    QMutexLocker locker(&this->dataMutex);
    return this->currentStep;
}

const double& Simulation::getCurrentTime() const
{
    QMutexLocker locker(&this->dataMutex);
    return this->currentTime;
}

const Scenery& Simulation::getScenery() const
{
    // No mutex needed - scenery object is not modified during simulation run loop
    return this->scenery;
}

const double& Simulation::getTimeStep() const
{
    // No mutex needed - timeStep is read-only during simulation execution
    return this->timeStep;
}

const double& Simulation::getTotalTime() const
{
    // No mutex needed - totalTime is read-only during simulation execution
    return this->totalTime;
}

const ulong& Simulation::getTotalSteps() const
{
    // No mutex needed - totalSteps is read-only during simulation execution
    return this->totalSteps;
}

const double& Simulation::getLogTime() const
{
    // No mutex needed - logTime is read-only during simulation execution
    return this->logTime;
}

const double& Simulation::getStepsPerSecond() const
{
    QMutexLocker locker(&this->dataMutex);
    return this->stepsPerSecond;
}

ulong Simulation::getEta() const
{
    QMutexLocker locker(&this->dataMutex);
    ulong remaningSteps = this->totalSteps - this->currentStep;

    if(this->stepsPerSecond > 0) {
        return remaningSteps / this->stepsPerSecond;
    }
    else {
        return 0;
    }
}

ulong Simulation::getEt() const
{
    QMutexLocker locker(&this->dataMutex);
    return this->et / 1000;
}

bool Simulation::isPaused() const
{
    QMutexLocker locker(&this->dataMutex);
    return this->paused;
}

bool Simulation::isStopped() const
{
    QMutexLocker locker(&this->dataMutex);
    return this->stoped;
}

bool Simulation::isPrimary() const
{
    QMutexLocker locker(&this->dataMutex);
    return this->primary;
}

bool Simulation::isInitialized() const
{
    QMutexLocker locker(&this->dataMutex);
    return this->initialized;
}

void Simulation::run()
{
    try {
        std::cout << "Simulation::run() started for simulation " << this->getId().toStdString() << std::endl;

        if(!this->isInitialized()) {
            emit this->newLog("Initilizing simulation's scenery");
            this->initialize();
            this->setInitialized(true);
            emit this->newLog("Simulation's scenery initialized");
        }

    const MaterialsManager& materialsManager = this->scenery.getMaterialsManager();

    std::vector<SimulationCL> simulationsCL = { this->getCL() };
    std::vector<SceneryCL>    sceneriesCL   = { this->scenery.getCL() };

    QVector<ParticleCL> particles = this->scenery.getObjectsManager().getParticlesCL(materialsManager);
    std::vector<ParticleCL> particlesCL = std::vector(particles.begin(), particles.end());

    QVector<FaceCL> faces = this->scenery.getObjectsManager().getFacesCL(materialsManager);
    std::vector<FaceCL> facesCL = std::vector(faces.begin(), faces.end());

    std::vector<MaterialsManagerCL> materialsManagerCL = { materialsManager.getCL() };

    OpenCLWrapper::Core openClCore(this->multiGPU);

    emit this->newLog("Loading OpenCL kernel");
    openClCore.addSourceFile("../Simulation.cpp.cl");
    emit this->newLog("OpenCL kernel loaded");

    emit this->newLog(QString("Total number of particles: %1").arg(particlesCL.size()));
    emit this->newLog(QString("Total number of faces: %1").arg(facesCL.size()));

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

        openClCore.run();
        openClCore.clearKernels();

	openClCore.syncDevicesBuffers(particlesCL);
	openClCore.syncDevicesBuffers<FaceCL>(facesCL);
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
        openClCore.addKernel("calculate_particles_neighborhood", particlesCL.size());
        openClCore.addArgument<ParticleCL>("calculate_particles_neighborhood", particlesCL);
        openClCore.addArgument<FaceCL>("calculate_particles_neighborhood", facesCL);
        openClCore.addArgument<MaterialsManagerCL>("calculate_particles_neighborhood", materialsManagerCL);
        openClCore.addArgument<SceneryCL>("calculate_particles_neighborhood", sceneriesCL);
        openClCore.addArgument<SimulationCL>("calculate_particles_neighborhood", simulationsCL);

        openClCore.addKernel("calculate_particle_to_particle", particlesCL.size());
        openClCore.addArgument<ParticleCL>("calculate_particle_to_particle", particlesCL);
        openClCore.addArgument<MaterialsManagerCL>("calculate_particle_to_particle", materialsManagerCL);
    }

    if(hasParticles && hasFaces) {
        openClCore.addKernel("calculate_faces_neighborhood", facesCL.size());
        openClCore.addArgument<FaceCL>("calculate_faces_neighborhood", facesCL);
        openClCore.addArgument<ParticleCL>("calculate_faces_neighborhood", particlesCL);
        openClCore.addArgument<MaterialsManagerCL>("calculate_faces_neighborhood", materialsManagerCL);
        openClCore.addArgument<SceneryCL>("calculate_faces_neighborhood", sceneriesCL);
        openClCore.addArgument<SimulationCL>("calculate_faces_neighborhood", simulationsCL);

        openClCore.addKernel("calculate_particle_to_face", particlesCL.size());
        openClCore.addArgument<ParticleCL>("calculate_particle_to_face", particlesCL);
        openClCore.addArgument<FaceCL>("calculate_particle_to_face", facesCL);
        openClCore.addArgument<MaterialsManagerCL>("calculate_particle_to_face", materialsManagerCL);

        openClCore.addKernel("calculate_face_to_particle", facesCL.size());
        openClCore.addArgument<FaceCL>("calculate_face_to_particle", facesCL);
        openClCore.addArgument<ParticleCL>("calculate_face_to_particle", particlesCL);
        openClCore.addArgument<MaterialsManagerCL>("calculate_face_to_particle", materialsManagerCL);
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

    this->setPaused(false);
    this->setStopped(false);

    uint frameSteps = this->frameTime / this->timeStep;

    bool ran = false;

    emit this->newLog("Simulation began");

    // Start the logger thread
    if (this->logger) {
        this->logger->start();
    }

    while ((this->getCurrentStep() <= this->totalSteps) && !this->isPaused() && !this->isStopped())
    {
        ran = true;
        this->setCurrentTime(this->timeStep * this->getCurrentStep());

        simulationsCL = { this->getCL() };
        openClCore.writeBuffer(simulationsCL);

        ulong currentStep = this->getCurrentStep();

        if(this->multiGPU || currentStep % frameSteps == 0) {
            openClCore.syncDevicesBuffers(particlesCL);
            openClCore.syncDevicesBuffers(facesCL);
        }

        if(currentStep % frameSteps == 0) {
            this->scenery.setParticlesCL(QVector<ParticleCL>(particlesCL.begin(), particlesCL.end()));
            this->scenery.setFacesCL(QVector<FaceCL>(facesCL.begin(), facesCL.end()));

            bool isDetailed = false;

	    if (this->isPrimary()) {
		isDetailed = currentStep % (frameSteps * this->detailedFramesDiv) == 0;
	    }

            emit this->newFrame(isDetailed);
        }

        openClCore.run();

        this->incrementCurrentStep();
    }

    // Decrement because the last loop dosen't happen.
    if(ran && !this->isPaused()) {
        this->decrementCurrentStep();
    }

    if(this->isPaused()) {
        openClCore.syncDevicesBuffers(particlesCL);
        openClCore.syncDevicesBuffers(facesCL);

        this->scenery.setParticlesCL(QVector<ParticleCL>(particlesCL.begin(), particlesCL.end()));
        this->scenery.setFacesCL(QVector<FaceCL>(facesCL.begin(), facesCL.end()));

        // Wait for all frames to be sent before declaring simulation paused
        emit this->newLog("Waiting for all frames to be sent...");
        RequestSender::getInstance().waitForAllFramesSent(this);

        emit this->newLog("Simulation paused");
    }

    if(this->isStopped())
        emit this->newLog("Simulation stopped");

    // Wait for all frames to be sent before declaring simulation ended
    emit this->newLog("Waiting for all frames to be sent...");
    RequestSender::getInstance().waitForAllFramesSent(this);

    emit this->newLog("Simulation ended");

    } catch (const std::exception& e) {
        std::cout << "Exception in Simulation::run(): " << e.what() << std::endl;
        emit this->newLog(QString("Simulation error: %1").arg(e.what()));
    } catch (...) {
        std::cout << "Unknown exception in Simulation::run()" << std::endl;
        emit this->newLog("Simulation error: Unknown exception occurred");
    }

    std::cout << "Simulation::run() finished for simulation " << this->getId().toStdString() << std::endl;
}

void Simulation::pause()
{
    this->setPaused(true);
}

void Simulation::stop()
{
    this->setStopped(true);

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

void Simulation::setCurrentTime(double currentTime)
{
    QMutexLocker locker(&this->dataMutex);
    this->currentTime = currentTime;
}

void Simulation::setCurrentStep(ulong currentStep)
{
    QMutexLocker locker(&this->dataMutex);
    this->currentStep = currentStep;
}

void Simulation::incrementCurrentStep()
{
    QMutexLocker locker(&this->dataMutex);
    this->currentStep++;
}

void Simulation::decrementCurrentStep()
{
    QMutexLocker locker(&this->dataMutex);
    this->currentStep--;
}

void Simulation::setStepsPerSecond(double stepsPerSecond)
{
    QMutexLocker locker(&this->dataMutex);
    this->stepsPerSecond = stepsPerSecond;
}

void Simulation::setEt(ulong et)
{
    QMutexLocker locker(&this->dataMutex);
    this->et = et;
}

void Simulation::setPaused(bool paused)
{
    QMutexLocker locker(&this->dataMutex);
    this->paused = paused;
}

void Simulation::setStopped(bool stopped)
{
    QMutexLocker locker(&this->dataMutex);
    this->stoped = stopped;
}

void Simulation::setInitialized(bool initialized)
{
    QMutexLocker locker(&this->dataMutex);
    this->initialized = initialized;
}


