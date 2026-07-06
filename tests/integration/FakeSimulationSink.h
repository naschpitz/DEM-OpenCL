#ifndef FAKE_SIMULATION_SINK_H
#define FAKE_SIMULATION_SINK_H

#include "SimulationSink.h"

#include <QVector>
#include <QString>
#include <QMutex>

#include "nlohmann/json.hpp"

// Records everything a Simulation announces, with no side effects: no
// ./framesData/ writes, no network sends, no DEM.ini lookup. Each captured
// frame snapshots the scenery JSON (same payload the production RequestSender
// would forward), so physics assertions can read particle positions per step.
struct CapturedFrame {
    ulong step;
    double time;
    bool detailed;
    nlohmann::json scenery;
};

struct CapturedLog {
    QString message;
};

class FakeSimulationSink : public SimulationSink
{
  public:
    QVector<CapturedFrame> frames;
    QVector<CapturedLog> logs;
    int waitCalls = 0;

    void onNewFrame(const Simulation* simulation, bool detailed) override;
    void onNewLog(const Simulation* simulation, const QString& message) override;
    void onWaitForAllFramesSent(const Simulation* simulation) override;

    void clear();

    int frameCount();
    int waitCallCount();

  private:
    QMutex mutex;
};

#endif // FAKE_SIMULATION_SINK_H
