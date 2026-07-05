#include "FakeSimulationSink.h"

#include "Simulation.h"

void FakeSimulationSink::onNewFrame(const Simulation* simulation, bool detailed)
{
    CapturedFrame frame;

    frame.step     = simulation->getCurrentStep();
    frame.time     = simulation->getCurrentTime();
    frame.detailed = detailed;
    frame.scenery  = simulation->getScenery().getJson(true);

    this->frames.append(frame);
}

void FakeSimulationSink::onNewLog(const Simulation* simulation, const QString& message)
{
    Q_UNUSED(simulation)

    this->logs.append({message});
}

void FakeSimulationSink::onWaitForAllFramesSent(const Simulation* simulation)
{
    Q_UNUSED(simulation)

    this->waitCalls++;
}

void FakeSimulationSink::clear()
{
    this->frames.clear();
    this->logs.clear();
    this->waitCalls = 0;
}
