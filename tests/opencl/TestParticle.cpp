#include "TestParticle.h"

#include "TestHelpers.h"

#include <QTest>

TestParticle::TestParticle()
{
  this->harness = nullptr;
}

void TestParticle::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available or test kernel compilation failed.");
}

void TestParticle::cleanupTestCase()
{
  delete this->harness;
  this->harness = nullptr;
}

void TestParticle::getClosestToAligned()
{
  std::vector<ParticleCL> particles = {makeParticleCL(0.0f, 0.0f, 0.0f, 1.0f), makeParticleCL(10.0f, 0.0f, 0.0f, 2.0f)};

  std::vector<cl_float4> outThis;
  std::vector<cl_float4> outOther;

  this->harness->runParticleGetClosestTo(particles, outThis, outOther);

  QCOMPARE(outThis[0].x, 1.0f);
  QCOMPARE(outThis[0].y, 0.0f);
  QCOMPARE(outThis[0].z, 0.0f);

  QCOMPARE(outOther[0].x, 8.0f);
  QCOMPARE(outOther[0].y, 0.0f);
  QCOMPARE(outOther[0].z, 0.0f);
}

void TestParticle::getClosestTo3D()
{
  std::vector<ParticleCL> particles = {makeParticleCL(0.0f, 0.0f, 0.0f, 1.0f), makeParticleCL(0.6f, 0.8f, 0.0f, 0.5f)};

  std::vector<cl_float4> outThis;
  std::vector<cl_float4> outOther;

  this->harness->runParticleGetClosestTo(particles, outThis, outOther);

  QCOMPARE(outThis[0].x, 0.6f);
  QCOMPARE(outThis[0].y, 0.8f);
  QCOMPARE(outThis[0].z, 0.0f);

  QCOMPARE(outOther[0].x, 0.3f);
  QCOMPARE(outOther[0].y, 0.4f);
  QCOMPARE(outOther[0].z, 0.0f);
}

void TestParticle::getClosestToOverlapping()
{
  std::vector<ParticleCL> particles = {makeParticleCL(0.0f, 0.0f, 0.0f, 5.0f), makeParticleCL(3.0f, 0.0f, 0.0f, 5.0f)};

  std::vector<cl_float4> outThis;
  std::vector<cl_float4> outOther;

  this->harness->runParticleGetClosestTo(particles, outThis, outOther);

  QCOMPARE(outThis[0].x, 5.0f);
  QCOMPARE(outThis[0].y, 0.0f);
  QCOMPARE(outThis[0].z, 0.0f);

  QCOMPARE(outOther[0].x, -2.0f);
  QCOMPARE(outOther[0].y, 0.0f);
  QCOMPARE(outOther[0].z, 0.0f);
}

void TestParticle::isInternalOutside()
{
  std::vector<ParticleCL> particles = {makeParticleCL(0.0f, 0.0f, 0.0f, 1.0f)};

  cl_float4 query = {5.0f, 0.0f, 0.0f, 0.0f};

  std::vector<cl_uint> result = this->harness->runParticleIsInternal(particles, {query});

  QCOMPARE(result[0], 0u);
}

void TestParticle::isInternalInside()
{
  std::vector<ParticleCL> particles = {makeParticleCL(0.0f, 0.0f, 0.0f, 1.0f)};

  cl_float4 query = {0.5f, 0.0f, 0.0f, 0.0f};

  std::vector<cl_uint> result = this->harness->runParticleIsInternal(particles, {query});

  QCOMPARE(result[0], 1u);
}

void TestParticle::isInternalBoundary()
{
  std::vector<ParticleCL> particles = {makeParticleCL(0.0f, 0.0f, 0.0f, 1.0f)};

  cl_float4 query = {1.0f, 0.0f, 0.0f, 0.0f};

  std::vector<cl_uint> result = this->harness->runParticleIsInternal(particles, {query});

  QCOMPARE(result[0], 0u);
}
