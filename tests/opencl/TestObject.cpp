#include "TestObject.h"

#include "TestHelpers.h"

#include <QTest>

TestObject::TestObject()
{
  this->harness = nullptr;
}

void TestObject::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available or test kernel compilation failed.");
}

void TestObject::cleanupTestCase()
{
  delete this->harness;
  this->harness = nullptr;
}

void TestObject::outsideFace()
{
  std::vector<FaceCL> cube = makeUnitCubeCL();
  std::vector<ParticleCL> particles = {makeParticleCL(3.0f, 0.5f, 0.5f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runObjectToParticle(cube, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 1.0f);
  QCOMPARE(outFace[0].y, 0.5f);
  QCOMPARE(outFace[0].z, 0.5f);
}

void TestObject::onFaceCenter()
{
  std::vector<FaceCL> cube = makeUnitCubeCL();
  std::vector<ParticleCL> particles = {makeParticleCL(1.0f, 0.5f, 0.5f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runObjectToParticle(cube, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 1.0f);
  QCOMPARE(outFace[0].y, 0.5f);
  QCOMPARE(outFace[0].z, 0.5f);
}

void TestObject::sharedEdge()
{
  std::vector<FaceCL> cube = makeUnitCubeCL();
  std::vector<ParticleCL> particles = {makeParticleCL(2.0f, 2.0f, 0.5f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runObjectToParticle(cube, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 1.0f);
  QCOMPARE(outFace[0].y, 1.0f);
  QCOMPARE(outFace[0].z, 0.5f);
}

void TestObject::sharedCorner()
{
  std::vector<FaceCL> cube = makeUnitCubeCL();
  std::vector<ParticleCL> particles = {makeParticleCL(2.0f, 2.0f, 2.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runObjectToParticle(cube, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 1.0f);
  QCOMPARE(outFace[0].y, 1.0f);
  QCOMPARE(outFace[0].z, 1.0f);
}

void TestObject::insideCube()
{
  std::vector<FaceCL> cube = makeUnitCubeCL();
  std::vector<ParticleCL> particles = {makeParticleCL(0.5f, 0.5f, 0.9f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runObjectToParticle(cube, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 0.5f);
  QCOMPARE(outFace[0].y, 0.5f);
  QCOMPARE(outFace[0].z, 1.0f);
}

void TestObject::farParticle()
{
  std::vector<FaceCL> cube = makeUnitCubeCL();
  std::vector<ParticleCL> particles = {makeParticleCL(0.5f, 0.5f, -10.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runObjectToParticle(cube, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 0.5f);
  QCOMPARE(outFace[0].y, 0.5f);
  QCOMPARE(outFace[0].z, 0.0f);
}
