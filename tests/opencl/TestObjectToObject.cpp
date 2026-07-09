#include "TestObjectToObject.h"

#include "TestHelpers.h"

#include <QTest>

#include <cmath>

TestObjectToObject::TestObjectToObject()
{
  this->harness = nullptr;
}

void TestObjectToObject::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available or test kernel compilation failed.");
}

void TestObjectToObject::cleanupTestCase()
{
  delete this->harness;
  this->harness = nullptr;
}

void TestObjectToObject::cornerToCorner()
{
  std::vector<FaceCL> cubeA = makeUnitCubeCL();
  std::vector<FaceCL> cubeB = makeUnitCubeCL();
  translateFaces(cubeB, 2.0f, 2.0f, 2.0f);

  cl_float4 closestA, closestB;
  float distance;

  this->harness->runObjectToObject(cubeA, cubeB, closestA, closestB, distance);

  QCOMPARE(distance, sqrtf(3.0f));
  QCOMPARE(closestA.x, 1.0f);
  QCOMPARE(closestA.y, 1.0f);
  QCOMPARE(closestA.z, 1.0f);
  QCOMPARE(closestB.x, 2.0f);
  QCOMPARE(closestB.y, 2.0f);
  QCOMPARE(closestB.z, 2.0f);
}

void TestObjectToObject::faceToFace()
{
  std::vector<FaceCL> cubeA = makeUnitCubeCL();
  std::vector<FaceCL> cubeB = makeUnitCubeCL();
  translateFaces(cubeB, 2.0f, 0.0f, 0.0f);

  cl_float4 closestA, closestB;
  float distance;

  this->harness->runObjectToObject(cubeA, cubeB, closestA, closestB, distance);

  QCOMPARE(distance, 1.0f);
}

void TestObjectToObject::edgeToEdge()
{
  std::vector<FaceCL> cubeA = makeUnitCubeCL();
  std::vector<FaceCL> cubeB = makeUnitCubeCL();
  translateFaces(cubeB, 2.0f, 2.0f, 0.0f);

  cl_float4 closestA, closestB;
  float distance;

  this->harness->runObjectToObject(cubeA, cubeB, closestA, closestB, distance);

  QCOMPARE(distance, sqrtf(2.0f));
}

void TestObjectToObject::overlapping()
{
  std::vector<FaceCL> cubeA = makeUnitCubeCL();
  std::vector<FaceCL> cubeB = makeUnitCubeCL();

  cl_float4 closestA, closestB;
  float distance;

  this->harness->runObjectToObject(cubeA, cubeB, closestA, closestB, distance);

  QVERIFY(distance < 1e-4f);
}

void TestObjectToObject::touchingFaces()
{
  std::vector<FaceCL> cubeA = makeUnitCubeCL();
  std::vector<FaceCL> cubeB = makeUnitCubeCL();
  translateFaces(cubeB, 1.0f, 0.0f, 0.0f);

  cl_float4 closestA, closestB;
  float distance;

  this->harness->runObjectToObject(cubeA, cubeB, closestA, closestB, distance);

  QVERIFY(distance < 1e-4f);
}

void TestObjectToObject::lShapeToCube()
{
  delete this->harness;
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available or test kernel compilation failed.");

  std::vector<FaceCL> lShape = makeLShapeCL();
  std::vector<FaceCL> cube = makeUnitCubeCL();
  translateFaces(cube, 3.0f, 0.0f, 0.0f);

  cl_float4 closestA, closestB;
  float distance;

  this->harness->runObjectToObject(lShape, cube, closestA, closestB, distance);

  QCOMPARE(distance, 1.0f);
}
