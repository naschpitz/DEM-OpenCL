#include "TestSimulationCollisions.h"

#include <sstream>
#include <string>

#include <QtTest>

#include "FakeSimulationSink.h"
#include "Simulation.h"
#include "SimulationTestHarness.h"

std::string TestSimulationCollisions::makeFloorSTL()
{
  // Two triangles forming a flat quad at z=0. Unlike a box, this has NO
  // opposite face — so when the particle penetrates, only the repulsive
  // force from the top surface acts (no cancellation from a bottom face).
  auto vertex = [](double x, double y, double z) {
    std::ostringstream os;
    os << "vertex " << x << " " << y << " " << z << "\n";
    return os.str();
  };

  auto facet = [&](const std::string& normal, const std::string& a, const std::string& b, const std::string& c) {
    std::ostringstream os;
    os << "facet normal " << normal << "\nouter loop\n" << a << b << c << "endloop\nendfacet\n";
    return os.str();
  };

  std::ostringstream stl;
  stl << "solid floor\n";
  stl << facet("0 0 1", vertex(-5.0, -5.0, 0.0), vertex(5.0, -5.0, 0.0), vertex(5.0, 5.0, 0.0));
  stl << facet("0 0 1", vertex(-5.0, -5.0, 0.0), vertex(5.0, 5.0, 0.0), vertex(-5.0, 5.0, 0.0));
  stl << "endsolid floor\n";

  return stl.str();
}

nlohmann::json TestSimulationCollisions::floorSimulationJson(double particleZ)
{
  nlohmann::json j;

  j["_id"] = "collisionSim";
  j["instance"] = "collisionInst";
  j["primary"] = true;
  j["url"] = "http://example.com";
  j["timeStep"] = 0.001;
  j["totalTime"] = 0.5;
  j["frameTime"] = 0.05;
  j["logTime"] = 0.1;
  j["detailedFramesDiv"] = 10;
  j["calcNeighTimeInt"] = 0.001;
  j["neighDistThresMult"] = 1.5;
  j["multiGPU"] = false;

  nlohmann::json scenery;
  scenery["_id"] = "collisionScenery";
  scenery["gravity"] = {0.0, 0.0, -9.81};

  nlohmann::json material;
  material["_id"] = "mat";
  material["material1"] = "mat";
  material["material2"] = "mat";
  material["distanceThreshold"] = 1.0;
  material["forceType"] = "inverse_quadratic";
  material["coefficients"] = {500.0, 0, 0, 0, 0, 0};
  scenery["materials"] = nlohmann::json::array({material});

  nlohmann::json particle;
  particle["_id"] = "particle";
  particle["material"] = "mat";
  particle["density"] = 1000.0;
  particle["dimensions"] = {{"spacing", 1.0}, {"length", {1.5, 1.5, 1.5}}};
  particle["position"] = {0.0, 0.0, particleZ};
  particle["velocity"] = {0.0, 0.0, 0.0};

  nlohmann::json floor;
  floor["_id"] = "floor";
  floor["material"] = "mat";
  floor["mass"] = 1000.0;
  floor["fixed"] = true;
  floor["position"] = {0.0, 0.0, 0.0};
  floor["velocity"] = {0.0, 0.0, 0.0};
  floor["stl"] = makeFloorSTL();

  nlohmann::json objects;
  objects["nonSolidObjects"] = nlohmann::json::array({particle});
  objects["solidObjects"] = nlohmann::json::array({floor});
  scenery["objects"] = objects;

  j["scenery"] = scenery;

  return j;
}

double TestSimulationCollisions::particleZ(const nlohmann::json& scenery)
{
  return scenery["objects"]["nonSolidObjects"][0]["particles"][0]["position"][2].get<double>();
}

TestSimulationCollisions::TestSimulationCollisions() {}

void TestSimulationCollisions::initTestCase() {}

void TestSimulationCollisions::particleDoesNotFallThroughFloor()
{
  SimulationTestHarness harness;

  const double initialZ = 1.0;

  Simulation simulation(floorSimulationJson(initialZ), &harness.sink);

  harness.runUntilFinished(&simulation);

  QVERIFY2(!harness.sink.frames.isEmpty(), "Expected at least one captured frame");

  for (const CapturedFrame& frame : harness.sink.frames) {
    const double z = particleZ(frame.scenery);

    QVERIFY2(z > 0.0, QString("Step %1: particle Z=%2 (expected above 0.0 — floor should prevent fall-through)")
                        .arg(static_cast<qulonglong>(frame.step))
                        .arg(z)
                        .toLocal8Bit()
                        .constData());
  }
}

void TestSimulationCollisions::particleInitiallyFallsTowardsFloor()
{
  SimulationTestHarness harness;

  const double initialZ = 1.0;

  Simulation simulation(floorSimulationJson(initialZ), &harness.sink);

  harness.runUntilFinished(&simulation);

  QVERIFY2(harness.sink.frames.size() >= 2, "Expected at least 2 frames");

  const double firstFrameZ = particleZ(harness.sink.frames[0].scenery);

  QVERIFY2(qAbs(firstFrameZ - initialZ) < 1e-4,
           QString("Frame 0: Z=%1 (expected initial Z=%2)").arg(firstFrameZ).arg(initialZ).toLocal8Bit().constData());

  const double secondFrameZ = particleZ(harness.sink.frames[1].scenery);

  QVERIFY2(secondFrameZ < firstFrameZ, QString("Frame 1: Z=%1 should be below frame 0 Z=%2 (particle should fall)")
                                         .arg(secondFrameZ)
                                         .arg(firstFrameZ)
                                         .toLocal8Bit()
                                         .constData());
}
