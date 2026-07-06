#include "TestSimulationContacts.h"

#include <QtTest>

#include "FakeSimulationSink.h"
#include "Simulation.h"
#include "SimulationTestHarness.h"

nlohmann::json TestSimulationContacts::contactSimulationJson(double posX, bool fixedA, bool fixedB)
{
    nlohmann::json j;

    j["_id"]               = "contactSim";
    j["instance"]          = "contactInst";
    j["primary"]           = true;
    j["url"]               = "http://example.com";
    j["timeStep"]          = 0.001;
    j["totalTime"]         = 0.05;
    j["frameTime"]         = 0.005;
    j["logTime"]           = 0.1;
    j["detailedFramesDiv"] = 10;
    j["calcNeighTimeInt"]  = 0.001;
    j["neighDistThresMult"]= 1.5;
    j["multiGPU"]          = false;

    nlohmann::json scenery;
    scenery["_id"]     = "contactScenery";
    scenery["gravity"] = { 0.0, 0.0, 0.0 };

    nlohmann::json material;
    material["_id"]              = "matA";
    material["material1"]        = "matA";
    material["material2"]        = "matA";
    material["distanceThreshold"]= 2.0;
    material["forceType"]        = "hooks_law";
    material["coefficients"]     = { 100.0, 0, 0, 0, 0, 0 };
    scenery["materials"] = nlohmann::json::array({ material });

    nlohmann::json objA;
    objA["_id"]      = "a";
    objA["material"] = "matA";
    objA["fixed"]    = fixedA;
    objA["density"]  = 1.0;
    objA["dimensions"] = { {"spacing", 1.0}, {"length", {1.5, 1.5, 1.5}} };
    objA["position"] = { -posX, 0.0, 0.0 };
    objA["velocity"] = { 0.0, 0.0, 0.0 };

    nlohmann::json objB;
    objB["_id"]      = "b";
    objB["material"] = "matA";
    objB["fixed"]    = fixedB;
    objB["density"]  = 1.0;
    objB["dimensions"] = { {"spacing", 1.0}, {"length", {1.5, 1.5, 1.5}} };
    objB["position"] = { posX, 0.0, 0.0 };
    objB["velocity"] = { 0.0, 0.0, 0.0 };

    nlohmann::json objects;
    objects["nonSolidObjects"] = nlohmann::json::array({ objA, objB });
    objects["solidObjects"]    = nlohmann::json::array();
    scenery["objects"] = objects;

    j["scenery"] = scenery;

    return j;
}

double TestSimulationContacts::particlePosX(const nlohmann::json& scenery, int obj)
{
    return scenery["objects"]["nonSolidObjects"][obj]["particles"][0]["position"][0].get<double>();
}

double TestSimulationContacts::particleVelX(const nlohmann::json& scenery, int obj)
{
    return scenery["objects"]["nonSolidObjects"][obj]["particles"][0]["velocity"][0].get<double>();
}

double TestSimulationContacts::particleVelY(const nlohmann::json& scenery, int obj)
{
    return scenery["objects"]["nonSolidObjects"][obj]["particles"][0]["velocity"][1].get<double>();
}

double TestSimulationContacts::particleVelZ(const nlohmann::json& scenery, int obj)
{
    return scenery["objects"]["nonSolidObjects"][obj]["particles"][0]["velocity"][2].get<double>();
}

void TestSimulationContacts::initTestCase()
{
}

void TestSimulationContacts::twoParticlesConserveMomentum()
{
    SimulationTestHarness harness;

    const double posX = 0.3;

    Simulation simulation(contactSimulationJson(posX, false, false), &harness.sink);

    harness.runUntilFinished(&simulation);

    QVERIFY2(!harness.sink.frames.isEmpty(), "Expected at least one captured frame");

    for (const CapturedFrame& frame : harness.sink.frames) {
        const double vxSum = particleVelX(frame.scenery, 0) + particleVelX(frame.scenery, 1);
        const double vySum = particleVelY(frame.scenery, 0) + particleVelY(frame.scenery, 1);
        const double vzSum = particleVelZ(frame.scenery, 0) + particleVelZ(frame.scenery, 1);

        QVERIFY2(qAbs(vxSum) < 1e-4,
                 QString("Step %1: momentum X sum=%2 (expected ~0)")
                     .arg(static_cast<qulonglong>(frame.step)).arg(vxSum).toLocal8Bit().constData());
        QVERIFY2(qAbs(vySum) < 1e-4,
                 QString("Step %1: momentum Y sum=%2 (expected ~0)")
                     .arg(static_cast<qulonglong>(frame.step)).arg(vySum).toLocal8Bit().constData());
        QVERIFY2(qAbs(vzSum) < 1e-4,
                 QString("Step %1: momentum Z sum=%2 (expected ~0)")
                     .arg(static_cast<qulonglong>(frame.step)).arg(vzSum).toLocal8Bit().constData());
    }
}

void TestSimulationContacts::twoParticlesRemainSymmetric()
{
    SimulationTestHarness harness;

    const double posX = 0.3;

    Simulation simulation(contactSimulationJson(posX, false, false), &harness.sink);

    harness.runUntilFinished(&simulation);

    QVERIFY2(!harness.sink.frames.isEmpty(), "Expected at least one captured frame");

    for (const CapturedFrame& frame : harness.sink.frames) {
        const double posA = particlePosX(frame.scenery, 0);
        const double posB = particlePosX(frame.scenery, 1);

        QVERIFY2(qAbs(posA + posB) < 1e-4,
                 QString("Step %1: posA=%2, posB=%3 (expected posA=-posB)")
                     .arg(static_cast<qulonglong>(frame.step)).arg(posA).arg(posB).toLocal8Bit().constData());

        const double velA = particleVelX(frame.scenery, 0);
        const double velB = particleVelX(frame.scenery, 1);

        QVERIFY2(qAbs(velA + velB) < 1e-4,
                 QString("Step %1: velA=%2, velB=%3 (expected velA=-velB)")
                     .arg(static_cast<qulonglong>(frame.step)).arg(velA).arg(velB).toLocal8Bit().constData());
    }
}

void TestSimulationContacts::fixedParticleStaysAtRest()
{
    SimulationTestHarness harness;

    const double posX = 0.3;

    // Object B (index 1) is fixed; object A (index 0) is free.
    Simulation simulation(contactSimulationJson(posX, false, true), &harness.sink);

    harness.runUntilFinished(&simulation);

    QVERIFY2(!harness.sink.frames.isEmpty(), "Expected at least one captured frame");

    for (const CapturedFrame& frame : harness.sink.frames) {
        QVERIFY2(qAbs(particlePosX(frame.scenery, 1) - posX) < 1e-4,
                 QString("Step %1: fixed particle X=%2 (expected %3)")
                     .arg(static_cast<qulonglong>(frame.step))
                     .arg(particlePosX(frame.scenery, 1)).arg(posX).toLocal8Bit().constData());

        QVERIFY2(qAbs(particleVelX(frame.scenery, 1)) < 1e-4,
                 QString("Step %1: fixed particle velX=%2 (expected 0)")
                     .arg(static_cast<qulonglong>(frame.step))
                     .arg(particleVelX(frame.scenery, 1)).toLocal8Bit().constData());
    }

    // The free particle (object 0, at -posX) must have been pushed further
    // in the -X direction by the repulsive contact force.
    const double lastFreeX = particlePosX(harness.sink.frames.last().scenery, 0);

    QVERIFY2(lastFreeX < -posX,
             QString("Free particle should have moved past -%1, got X=%2")
                 .arg(posX).arg(lastFreeX).toLocal8Bit().constData());
}
