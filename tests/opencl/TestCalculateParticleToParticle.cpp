#include "TestCalculateParticleToParticle.h"

#include "TestHelpers.h"

#include <QTest>

TestCalculateParticleToParticle::TestCalculateParticleToParticle() {}

void TestCalculateParticleToParticle::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available");
}

void TestCalculateParticleToParticle::cleanupTestCase()
{
  delete this->harness;
}

void TestCalculateParticleToParticle::overlappingRepel()
{
  ParticleCL thisP = makeParticleCL(0.0f, 0.0f, 0.0f, 0.5f);
  ParticleCL otherP = makeParticleCL(0.3f, 0.0f, 0.0f, 0.5f);

  MaterialCL mat = makeMaterialCL(1, 100.0f, 2.0f);

  cl_float4 force;
  cl_float4 torque;

  this->harness->runParticleToParticleWorker(thisP, otherP, mat, force, torque);

  QCOMPARE(force.x, -70.0f);
  QCOMPARE(force.y, 0.0f);
  QCOMPARE(force.z, 0.0f);
  QCOMPARE(torque.x, 0.0f);
  QCOMPARE(torque.y, 0.0f);
  QCOMPARE(torque.z, 0.0f);
}

void TestCalculateParticleToParticle::separatedAttract()
{
  ParticleCL thisP = makeParticleCL(0.0f, 0.0f, 0.0f, 0.5f);
  ParticleCL otherP = makeParticleCL(2.0f, 0.0f, 0.0f, 0.5f);

  MaterialCL mat = makeMaterialCL(1, 100.0f, 2.0f);

  cl_float4 force;
  cl_float4 torque;

  this->harness->runParticleToParticleWorker(thisP, otherP, mat, force, torque);

  QCOMPARE(force.x, 100.0f);
  QCOMPARE(force.y, 0.0f);
  QCOMPARE(force.z, 0.0f);
  QCOMPARE(torque.x, 0.0f);
  QCOMPARE(torque.y, 0.0f);
  QCOMPARE(torque.z, 0.0f);
}

void TestCalculateParticleToParticle::beyondThreshold()
{
  ParticleCL thisP = makeParticleCL(0.0f, 0.0f, 0.0f, 0.5f);
  ParticleCL otherP = makeParticleCL(2.0f, 0.0f, 0.0f, 0.5f);

  MaterialCL mat = makeMaterialCL(1, 100.0f, 0.5f);

  cl_float4 force;
  cl_float4 torque;

  this->harness->runParticleToParticleWorker(thisP, otherP, mat, force, torque);

  QCOMPARE(force.x, 0.0f);
  QCOMPARE(force.y, 0.0f);
  QCOMPARE(force.z, 0.0f);
  QCOMPARE(torque.x, 0.0f);
  QCOMPARE(torque.y, 0.0f);
  QCOMPARE(torque.z, 0.0f);
}
