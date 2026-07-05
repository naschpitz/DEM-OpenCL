#ifndef TEST_SIMULATION_CONFIG_H
#define TEST_SIMULATION_CONFIG_H

#include <QObject>

#include "nlohmann/json.hpp"

// Phase 1 integration tests: Simulation construction, JSON contract, getCL()
// round-trip, and the running/paused/stopped/done state machine. No GPU and no
// event loop required — these never call Simulation::start(), so they are fast
// and safe to run in CI. They lock the JSON↔field contract between the HTTP
// layer and the device, which is currently untested by the unit suite.
class TestSimulationConfig : public QObject
{
    Q_OBJECT

    private:
        nlohmann::json validSimulationJson();

    private slots:
        void constructor();
        void getCL();
        void missingRequiredFields();
        void totalStepsComputation();
        void stateMachine();
};

#endif // TEST_SIMULATION_CONFIG_H
