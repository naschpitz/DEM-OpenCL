#include "SimulationLogger.h"
#include "Simulation.h"
#include "RequestSender.h"

#include <QDateTime>
#include <QMutexLocker>
#include <iostream>

SimulationLogger::SimulationLogger(Simulation* simulation, QObject* parent)
    : QObject(parent)
{
    this->simulation = simulation;
    this->logTimer = new QTimer(this);

    this->previousStep = 0;
    this->previousStepsPerSecond = 0.0;
    this->previousEt = 0;
    this->running = false;

    // Connect timer to our slot
    connect(this->logTimer, &QTimer::timeout, this, &SimulationLogger::onLogTimerTimeout);

    // Connect to simulation finished signal
    connect(this->simulation, &QThread::finished, this, &SimulationLogger::onSimulationFinished);

    // Set timer interval to check every 100ms (responsive but not too frequent)
    this->logTimer->setInterval(100);

    // Initialize last log time
    this->lastLogTime = QDateTime::currentDateTime();
    this->previousStep = this->simulation->getCurrentStep();
}

SimulationLogger::~SimulationLogger()
{
    this->stop();
}

void SimulationLogger::start()
{
    QMutexLocker locker(&this->mutex);

    if (this->running) {
        return; // Already running
    }

    this->running = true;

    // Initialize logging state
    this->lastLogTime = QDateTime::currentDateTime();
    this->previousStep = this->simulation->getCurrentStep();

    // Start the timer
    this->logTimer->start();
}

void SimulationLogger::stop()
{
    QMutexLocker locker(&this->mutex);

    if (!this->running) {
        return; // Already stopped
    }

    this->running = false;

    // Stop the timer
    this->logTimer->stop();
}

bool SimulationLogger::isRunning() const
{
    QMutexLocker locker(&this->mutex);
    return this->running;
}

void SimulationLogger::onLogTimerTimeout()
{
    if (!this->running || !this->simulation) {
        return;
    }

    // Check if simulation is still running
    if (this->simulation->isStopped() || this->simulation->isPaused()) {
        return;
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    ulong mSecElapsed = this->lastLogTime.msecsTo(currentTime);
    ulong currentStep = this->simulation->getCurrentStep();
    ulong totalSteps = this->simulation->getTotalSteps();

    // Get logTime from simulation (convert from seconds to milliseconds)
    double logTimeSeconds = this->simulation->getLogTime();
    ulong logTimeMs = static_cast<ulong>(logTimeSeconds * 1000);
    
    // Check if it's time to log or if we've reached the final step
    if (mSecElapsed > logTimeMs || currentStep == totalSteps) {
        this->updateLogData();
    }
}

void SimulationLogger::updateLogData()
{
    if (!this->simulation) {
        return;
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    ulong mSecElapsed = this->lastLogTime.msecsTo(currentTime);
    ulong currentStep = this->simulation->getCurrentStep();

    // Calculate steps per second
    ulong numSteps = currentStep - this->previousStep;
    double stepsPerSecond = 0.0;

    if (mSecElapsed > 0) {
        stepsPerSecond = (static_cast<double>(numSteps) / mSecElapsed) * 1000.0;
    }

    // Update elapsed time
    ulong newEt = this->previousEt + mSecElapsed;
    
    // Update state for next calculation
    this->previousStep = currentStep;
    this->previousStepsPerSecond = stepsPerSecond;
    this->previousEt = newEt;
    this->lastLogTime = currentTime;

    // Update simulation data directly and send log
    this->simulation->setStepsPerSecond(stepsPerSecond);
    this->simulation->setEt(newEt);

    // Send log directly to RequestSender
    RequestSender::getInstance().newLog("New simulation log");
}

void SimulationLogger::onSimulationFinished()
{
    // Perform final log update
    this->updateLogData();

    // Stop logging
    this->stop();

    std::cout << "SimulationLogger: Simulation finished, stopping logger" << std::endl;
}
