#include "TestCalculateFaceToParticle.h"

#include "TestHelpers.h"

#include <QTest>

TestCalculateFaceToParticle::TestCalculateFaceToParticle() {}

void TestCalculateFaceToParticle::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available");
}

void TestCalculateFaceToParticle::cleanupTestCase()
{
  delete this->harness;
}

void TestCalculateFaceToParticle::particleAboveFace()
{
  cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v2 = {5.0f, 5.0f, 0.0f, 0.0f};
  FaceCL face = makeFaceCL(v0, v1, v2);

  ParticleCL particle = makeParticleCL(0.0f, 0.0f, 1.0f, 0.5f);
  particle.neighborhood.numFaces = 1;

  MaterialCL mat = makeMaterialCL(1, 100.0f, 2.0f);

  cl_float4 force;
  cl_float4 torque;

  this->harness->runFaceToParticleWorker(face, particle, mat, force, torque);

  // Hooke's law: distance = (0, 0, 0.5), force = -k * distance = (0, 0, -50)
  // totalForce = -force = (0, 0, 50), divided by numFaces=1
  QCOMPARE(force.x, 0.0f);
  QCOMPARE(force.y, 0.0f);
  QCOMPARE(force.z, 50.0f);
  QCOMPARE(torque.x, 0.0f);
  QCOMPARE(torque.y, 0.0f);
  QCOMPARE(torque.z, 0.0f);
}

void TestCalculateFaceToParticle::beyondThreshold()
{
  cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v2 = {5.0f, 5.0f, 0.0f, 0.0f};
  FaceCL face = makeFaceCL(v0, v1, v2);

  ParticleCL particle = makeParticleCL(0.0f, 0.0f, 1.0f, 0.5f);
  particle.neighborhood.numFaces = 1;

  MaterialCL mat = makeMaterialCL(1, 100.0f, 0.3f);

  cl_float4 force;
  cl_float4 torque;

  this->harness->runFaceToParticleWorker(face, particle, mat, force, torque);

  // Distance 0.5 > threshold 0.3 -> no force
  QCOMPARE(force.x, 0.0f);
  QCOMPARE(force.y, 0.0f);
  QCOMPARE(force.z, 0.0f);
  QCOMPARE(torque.x, 0.0f);
  QCOMPARE(torque.y, 0.0f);
  QCOMPARE(torque.z, 0.0f);
}

void TestCalculateFaceToParticle::numFacesDivision()
{
  cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v2 = {5.0f, 5.0f, 0.0f, 0.0f};
  FaceCL face = makeFaceCL(v0, v1, v2);

  ParticleCL particle = makeParticleCL(0.0f, 0.0f, 1.0f, 0.5f);
  particle.neighborhood.numFaces = 2;

  MaterialCL mat = makeMaterialCL(1, 100.0f, 2.0f);

  cl_float4 force;
  cl_float4 torque;

  this->harness->runFaceToParticleWorker(face, particle, mat, force, torque);

  // totalForce = (0, 0, 50), divided by numFaces=2 -> (0, 0, 25)
  QCOMPARE(force.x, 0.0f);
  QCOMPARE(force.y, 0.0f);
  QCOMPARE(force.z, 25.0f);
  QCOMPARE(torque.x, 0.0f);
  QCOMPARE(torque.y, 0.0f);
  QCOMPARE(torque.z, 0.0f);
}
