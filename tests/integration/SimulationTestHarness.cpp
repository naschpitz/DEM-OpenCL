#include "SimulationTestHarness.h"

#include <QEventLoop>

#include "Simulation.h"

void SimulationTestHarness::runUntilFinished(Simulation* simulation)
{
  QEventLoop loop;

  // Snapshot the terminal state on finished(), then quit the loop. The
  // Simulation's own selfDelete() slot (connected in its constructor on the
  // same finished() signal) runs first and only schedules a deferred
  // deleteLater(), so the Simulation is still alive when this lambda runs.
  QObject::connect(simulation, &QThread::finished, [&]() {
    this->finalStep = simulation->getCurrentStep();
    this->finalTime = simulation->getCurrentTime();
    loop.quit();
  });

  simulation->start();
  loop.exec();
}
