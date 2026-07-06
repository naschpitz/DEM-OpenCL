#ifndef SIMULATION_TEST_HARNESS_H
#define SIMULATION_TEST_HARNESS_H

#include "FakeSimulationSink.h"

class Simulation;

// RAII-ish harness for running a Simulation under test.
//
// Simulation emits newFrame/newLog via BlockingQueuedConnection, which means
// the thread that constructed the Simulation MUST spin a QEventLoop while the
// worker thread runs — plain Simulation::wait() deadlocks because the main
// thread can't deliver the blocking slots. runUntilFinished() runs that event
// loop until the worker emits QThread::finished().
//
// finalStep / finalTime are captured from the finished() signal (before
// selfDelete()'s deleteLater() reaps the Simulation on the next loop turn), so
// they are safe to read after runUntilFinished() returns. The Simulation
// object itself must NOT be touched afterwards — it is scheduled for deletion.
class SimulationTestHarness
{
  public:
    FakeSimulationSink sink;

    ulong finalStep = 0;
    double finalTime = 0.0;

    void runUntilFinished(Simulation* simulation);
};

#endif // SIMULATION_TEST_HARNESS_H
