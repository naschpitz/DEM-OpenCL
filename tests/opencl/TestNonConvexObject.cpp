#include "TestNonConvexObject.h"

#include "TestHelpers.h"

#include <QTest>

TestNonConvexObject::TestNonConvexObject()
{
  this->harness = nullptr;
}

void TestNonConvexObject::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available or test kernel compilation failed.");
}

void TestNonConvexObject::cleanupTestCase()
{
  delete this->harness;
  this->harness = nullptr;
}

void TestNonConvexObject::pocketInnerWallY()
{
  std::vector<FaceCL> lShape = makeLShapeCL();
  std::vector<ParticleCL> particles = {makeParticleCL(1.3f, 1.2f, 0.5f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runObjectToParticle(lShape, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 1.3f);
  QCOMPARE(outFace[0].y, 1.0f);
  QCOMPARE(outFace[0].z, 0.5f);
}

void TestNonConvexObject::outerHull()
{
  std::vector<FaceCL> lShape = makeLShapeCL();
  std::vector<ParticleCL> particles = {makeParticleCL(3.0f, 0.5f, 0.5f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runObjectToParticle(lShape, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 2.0f);
  QCOMPARE(outFace[0].y, 0.5f);
  QCOMPARE(outFace[0].z, 0.5f);
}

void TestNonConvexObject::equidistantInnerFaces()
{
  std::vector<FaceCL> lShape = makeLShapeCL();
  std::vector<ParticleCL> particles = {makeParticleCL(1.5f, 1.5f, 0.5f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runObjectToParticle(lShape, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 1.0f);
  QCOMPARE(outFace[0].y, 1.5f);
  QCOMPARE(outFace[0].z, 0.5f);
}

void TestNonConvexObject::pocketInnerWallX()
{
  std::vector<FaceCL> lShape = makeLShapeCL();
  std::vector<ParticleCL> particles = {makeParticleCL(1.2f, 1.3f, 0.5f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runObjectToParticle(lShape, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 1.0f);
  QCOMPARE(outFace[0].y, 1.3f);
  QCOMPARE(outFace[0].z, 0.5f);
}
