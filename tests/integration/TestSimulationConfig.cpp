#include "TestSimulationConfig.h"

#include <QtTest>

#include "FakeSimulationSink.h"
#include "Simulation.h"

// Minimal valid Simulation JSON. The scenery is the smallest payload Scenery()
// accepts: _id + gravity + an empty materials array + an empty objects object
// (ObjectsManager swallows missing keys via catch(...)). No GPU, no start().
nlohmann::json TestSimulationConfig::validSimulationJson()
{
    return nlohmann::json::parse(R"({
        "_id": "sim1",
        "instance": "inst1",
        "primary": true,
        "url": "http://example.com",
        "timeStep": 0.001,
        "totalTime": 1.0,
        "frameTime": 0.01,
        "logTime": 0.1,
        "detailedFramesDiv": 10,
        "calcNeighTimeInt": 0.01,
        "neighDistThresMult": 1.5,
        "multiGPU": false,
        "scenery": {
            "_id": "scenery1",
            "gravity": [0, 0, -9.81],
            "materials": [],
            "objects": {}
        }
    })");
}

void TestSimulationConfig::constructor()
{
    FakeSimulationSink sink;
    nlohmann::json json = this->validSimulationJson();

    Simulation simulation(json, &sink);

    QCOMPARE(simulation.getId(),           QString("sim1"));
    QCOMPARE(simulation.getInstance(),     QString("inst1"));
    QCOMPARE(simulation.isPrimary(),       true);
    QCOMPARE(simulation.getInterfaceUrl(), QString("http://example.com"));
    QCOMPARE(simulation.getTimeStep(),     0.001);
    QCOMPARE(simulation.getTotalTime(),    1.0);
    QCOMPARE(simulation.getScenery().getId(), QString("scenery1"));

    QVERIFY(simulation.getCurrentTime()  == 0.0);
    QVERIFY(simulation.getCurrentStep()  == static_cast<ulong>(0));
}

void TestSimulationConfig::getCL()
{
    FakeSimulationSink sink;
    Simulation simulation(this->validSimulationJson(), &sink);

    SimulationCL cl = simulation.getCL();

    QCOMPARE(cl.currentStep,        static_cast<cl_ulong>(0));
    QCOMPARE(cl.calcNeighStepsInt,  static_cast<cl_ulong>(10));
    QCOMPARE(cl.currentTime,        0.0f);
    QCOMPARE(cl.timeStep,           0.001f);
    QCOMPARE(cl.totalTime,          1.0f);
    QCOMPARE(cl.frameTime,          0.01f);
    QCOMPARE(cl.neighDistThresMult, 1.5);
}

void TestSimulationConfig::missingRequiredFields()
{
    // Every field Simulation() reads via .at() must produce a runtime_error
    // with a stable, identifiable message. Data-driven over the full set so the
    // contract is locked end-to-end.
    struct Case { std::string field; std::string message; };
    const QVector<Case> cases = {
        {"_id",                "Missing '_id' field in Simulation"},
        {"instance",           "Missing 'instance' field in Simulation"},
        {"primary",            "Missing 'primary' field in Simulation"},
        {"url",                "Missing 'url' field in Simulation"},
        {"timeStep",           "Missing 'timeStep' field in Simulation"},
        {"totalTime",          "Missing 'totalTime' field in Simulation"},
        {"frameTime",          "Missing 'frameTime' field in Simulation"},
        {"logTime",            "Missing 'logTime' field in Simulation"},
        {"detailedFramesDiv",  "Missing 'detailedFramesDiv' field in Simulation"},
        {"calcNeighTimeInt",   "Missing 'calcNeighTimeInt' field in Simulation"},
        {"neighDistThresMult", "Missing 'neighDistThresMult' field in Simulation"},
        {"multiGPU",           "Missing 'multiGPU' field in Simulation"},
        {"scenery",            "Missing 'scenery' field in Simulation"},
    };

    FakeSimulationSink sink;

    for (const Case& c : cases) {
        nlohmann::json json = this->validSimulationJson();
        json.erase(c.field);

        try {
            Simulation simulation(json, &sink);
            QFAIL(QString("Expected std::runtime_error for missing '%1', but no "
                          "exception was thrown").arg(QString::fromStdString(c.field)).toLocal8Bit().constData());
        }

        catch (const std::runtime_error& e) {
            QVERIFY2(QString(e.what()) == QString::fromStdString(c.message),
                     QString("Field '%1': expected '%2' but got '%3'")
                        .arg(QString::fromStdString(c.field))
                        .arg(QString::fromStdString(c.message))
                        .arg(e.what()).toLocal8Bit().constData());
        }
    }
}

void TestSimulationConfig::totalStepsComputation()
{
    FakeSimulationSink sink;
    Simulation simulation(this->validSimulationJson(), &sink);

    // totalSteps = qRound(totalTime / timeStep) = qRound(1.0 / 0.001) = 1000.
    QCOMPARE(simulation.getTotalSteps(), static_cast<ulong>(1000));
}

void TestSimulationConfig::stateMachine()
{
    FakeSimulationSink sink;
    Simulation simulation(this->validSimulationJson(), &sink);

    // Freshly constructed: not started yet, but the boolean model reports
    // "running" because nothing is paused/stopped/done. This locks the current
    // semantics (the config tests document behavior, they do not redefine it).
    QVERIFY2(simulation.isRunning(),
             "Freshly constructed Simulation should report isRunning()=true");
    QVERIFY(!simulation.isPaused());
    QVERIFY(!simulation.isStopped());
    QVERIFY(!simulation.isDone());

    simulation.pause();
    QVERIFY(simulation.isPaused());
    QVERIFY(!simulation.isRunning());
    QVERIFY(!simulation.isStopped());
    QVERIFY(!simulation.isDone());

    // pause()/stop() are one-way — there is no resume().
    simulation.stop();
    QVERIFY(simulation.isStopped());
    QVERIFY(!simulation.isRunning());
}
