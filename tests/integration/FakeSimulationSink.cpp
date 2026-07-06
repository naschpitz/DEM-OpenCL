#include "FakeSimulationSink.h"

#include <QMutexLocker>

#include "Simulation.h"

void FakeSimulationSink::onNewFrame(const Simulation* simulation, bool detailed)
{
  QMutexLocker locker(&this->mutex);

  CapturedFrame frame;

  frame.step = simulation->getCurrentStep();
  frame.time = simulation->getCurrentTime();
  frame.detailed = detailed;
  frame.scenery = simulation->getScenery().getJson(true);

  this->frames.append(frame);
}

void FakeSimulationSink::onNewLog(const Simulation* simulation, const QString& message)
{
  Q_UNUSED(simulation)
  QMutexLocker locker(&this->mutex);

  this->logs.append({message});
}

void FakeSimulationSink::onWaitForAllFramesSent(const Simulation* simulation)
{
  Q_UNUSED(simulation)
  QMutexLocker locker(&this->mutex);

  this->waitCalls++;
}

void FakeSimulationSink::clear()
{
  QMutexLocker locker(&this->mutex);

  this->frames.clear();
  this->logs.clear();
  this->waitCalls = 0;
}

int FakeSimulationSink::frameCount()
{
  QMutexLocker locker(&this->mutex);
  return this->frames.size();
}

int FakeSimulationSink::waitCallCount()
{
  QMutexLocker locker(&this->mutex);
  return this->waitCalls;
}
