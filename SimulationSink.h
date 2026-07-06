#ifndef SIMULATION_SINK_H
#define SIMULATION_SINK_H

#include <QString>

class Simulation;

// Abstract sink for the frame/log/flush callbacks a Simulation emits as it runs.
//
// Simulation owns the physics loop and announces progress (frames, logs, end of
// run) through this interface. The production implementation is RequestSender,
// which forwards frames/logs to the remote interface. Integration tests inject a
// fake sink so a Simulation can run to completion without the RequestSender
// singleton (and therefore without DEM.ini, ./framesData/, or a network).
//
// Every method receives the originating Simulation* explicitly so sinks never
// need QObject::sender() to recover the source.
class SimulationSink
{
  public:
    virtual ~SimulationSink() = default;

    virtual void onNewFrame(const Simulation* simulation, bool detailed) = 0;
    virtual void onNewLog(const Simulation* simulation, const QString& message) = 0;
    virtual void onWaitForAllFramesSent(const Simulation* simulation) = 0;
};

#endif // SIMULATION_SINK_H
