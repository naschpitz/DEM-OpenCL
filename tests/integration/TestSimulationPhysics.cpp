#include "TestSimulationPhysics.h"

#include <QtTest>

#include "FakeSimulationSink.h"
#include "Simulation.h"
#include "SimulationTestHarness.h"

nlohmann::json TestSimulationPhysics::physicsSimulationJson(double gx, double gy, double gz, double vx, double vy,
                                                            double vz)
{
  // One particle: spacing=1, length=[1.5,1.5,1.5]. The close-packing builder
  // (NonSolidObject::buildObject) places the first sphere at (0.5,0.5,0.5) and
  // the second at x = 3*radius + radius = 2.0 > 1.5, so exactly one survives.
  // setPosition() then centers the object: the single particle lands at the
  // object's "position" ([0,0,0] here), so originalPosition = (0,0,0).
  nlohmann::json j;

  j["_id"] = "simPhysics";
  j["instance"] = "instPhysics";
  j["primary"] = true;
  j["url"] = "http://example.com";
  j["timeStep"] = 0.001;
  j["totalTime"] = 0.1;
  j["frameTime"] = 0.01;
  j["logTime"] = 0.1;
  j["detailedFramesDiv"] = 10;
  j["calcNeighTimeInt"] = 0.01;
  j["neighDistThresMult"] = 1.5;
  j["multiGPU"] = false;

  nlohmann::json scenery;
  scenery["_id"] = "sceneryPhysics";
  scenery["gravity"] = {gx, gy, gz};

  nlohmann::json material;
  material["_id"] = "matA";
  material["material1"] = "matA";
  material["material2"] = "matA";
  material["distanceThreshold"] = 1.0;
  material["forceType"] = "hooks_law";
  material["dragForceType"] = "linear";
  material["coefficients"] = {0, 0, 0, 0, 0, 0};
  material["dragCoefficients"] = {0, 0, 0, 0, 0, 0};
  scenery["materials"] = nlohmann::json::array({material});

  nlohmann::json obj;
  obj["_id"] = "obj1";
  obj["material"] = "matA";
  obj["fixed"] = false;
  obj["density"] = 1.0;
  obj["dimensions"] = {{"spacing", 1.0}, {"length", {1.5, 1.5, 1.5}}};
  obj["position"] = {0.0, 0.0, 0.0};
  obj["velocity"] = {vx, vy, vz};

  nlohmann::json objects;
  objects["nonSolidObjects"] = nlohmann::json::array({obj});
  objects["solidObjects"] = nlohmann::json::array();
  scenery["objects"] = objects;

  j["scenery"] = scenery;

  return j;
}

double TestSimulationPhysics::particleX(const nlohmann::json& scenery, int particle)
{
  return scenery["objects"]["nonSolidObjects"][0]["particles"][particle]["position"][0].get<double>();
}

double TestSimulationPhysics::particleY(const nlohmann::json& scenery, int particle)
{
  return scenery["objects"]["nonSolidObjects"][0]["particles"][particle]["position"][1].get<double>();
}

double TestSimulationPhysics::particleZ(const nlohmann::json& scenery, int particle)
{
  return scenery["objects"]["nonSolidObjects"][0]["particles"][particle]["position"][2].get<double>();
}

void TestSimulationPhysics::initTestCase()
{
  // The physics slots each construct a Simulation (which builds the OpenCL
  // device + kernels). If there is no GPU the first slot's construction will
  // fail with a clear OpenCL error. This slot exists so a device probe can be
  // added later without reshuffling the suite.
}

void TestSimulationPhysics::stationaryParticleDrifts()
{
  SimulationTestHarness harness;

  Simulation simulation(physicsSimulationJson(0.0, 0.0, 0.0, // gravity
                                              0.0, 0.0, 0.0), // velocity
                        &harness.sink);

  harness.runUntilFinished(&simulation);

  QVERIFY2(!harness.sink.frames.isEmpty(), "Expected at least one captured frame");
  QVERIFY2(harness.finalStep == static_cast<ulong>(100), QString("Expected simulation to run to step 100, got %1")
                                                           .arg(static_cast<qulonglong>(harness.finalStep))
                                                           .toLocal8Bit()
                                                           .constData());

  const nlohmann::json& firstFrame = harness.sink.frames.first().scenery;
  const nlohmann::json& lastFrame = harness.sink.frames.last().scenery;

  // Initial state (frame at step 0) must be the rest position.
  QCOMPARE(particleX(firstFrame), 0.0);
  QCOMPARE(particleY(firstFrame), 0.0);
  QCOMPARE(particleZ(firstFrame), 0.0);

  // After 100 steps with no forces, the particle must not have drifted.
  // Verlet is exact for zero acceleration (double-precision internally), so
  // the float32 readback is 0.0 to within rounding.
  QVERIFY2(qAbs(particleX(lastFrame)) < 1e-5f, "Particle drifted in X under zero force");
  QVERIFY2(qAbs(particleY(lastFrame)) < 1e-5f, "Particle drifted in Y under zero force");
  QVERIFY2(qAbs(particleZ(lastFrame)) < 1e-5f, "Particle drifted in Z under zero force");
}

void TestSimulationPhysics::particleMovesAtConstantVelocity()
{
  SimulationTestHarness harness;

  const double vx = 1.0;
  const double dt = 0.001; // timeStep — capture before run (Simulation is reaped after).

  Simulation simulation(physicsSimulationJson(0.0, 0.0, 0.0, // gravity
                                              vx, 0.0, 0.0), // velocity
                        &harness.sink);

  harness.runUntilFinished(&simulation);

  QVERIFY2(!harness.sink.frames.isEmpty(), "Expected at least one captured frame");

  // Verlet is exact for zero acceleration: x_n = x_0 + v_0 * (n*dt). Frame at
  // step N shows state after N integration steps, so for each frame
  // x == v_0 * (step * dt). Check every captured frame, not just the last.
  for (const CapturedFrame& frame : harness.sink.frames) {
    const double expectedX = vx * (static_cast<double>(frame.step) * dt);
    const double actualX = particleX(frame.scenery);

    QVERIFY2(qAbs(static_cast<float>(actualX - expectedX)) < 1e-4f, QString("Step %1: expected x=%2, got x=%3")
                                                                      .arg(static_cast<qulonglong>(frame.step))
                                                                      .arg(expectedX)
                                                                      .arg(actualX)
                                                                      .toLocal8Bit()
                                                                      .constData());

    // Y and Z stay at rest.
    QVERIFY2(qAbs(particleY(frame.scenery)) < 1e-4f, "Non-zero Y under X-only velocity");
    QVERIFY2(qAbs(particleZ(frame.scenery)) < 1e-4f, "Non-zero Z under X-only velocity");
  }
}

void TestSimulationPhysics::particleFallsUnderGravity()
{
  SimulationTestHarness harness;

  const double gz = -9.81;
  const double dt = 0.001; // timeStep — capture before run (Simulation is reaped after).

  Simulation simulation(physicsSimulationJson(0.0, 0.0, gz, // gravity
                                              0.0, 0.0, 0.0), // velocity
                        &harness.sink);

  harness.runUntilFinished(&simulation);

  QVERIFY2(!harness.sink.frames.isEmpty(), "Expected at least one captured frame");

  // Discrete Verlet closed form with the sim's seed (see header comment):
  //   z_n = 0.5 * a * dt^2 * n * (n+1)
  // Verify against every captured frame.
  for (const CapturedFrame& frame : harness.sink.frames) {
    const double n = static_cast<double>(frame.step);
    const double expectedZ = 0.5 * gz * dt * dt * n * (n + 1.0);
    const double actualZ = particleZ(frame.scenery);

    QVERIFY2(qAbs(static_cast<float>(actualZ - expectedZ)) < 1e-4f, QString("Step %1: expected z=%2, got z=%3")
                                                                      .arg(static_cast<qulonglong>(frame.step))
                                                                      .arg(expectedZ)
                                                                      .arg(actualZ)
                                                                      .toLocal8Bit()
                                                                      .constData());

    // X and Y stay at rest (gravity is Z-only).
    QVERIFY2(qAbs(particleX(frame.scenery)) < 1e-4f, "Non-zero X under Z-only gravity");
    QVERIFY2(qAbs(particleY(frame.scenery)) < 1e-4f, "Non-zero Y under Z-only gravity");
  }
}
