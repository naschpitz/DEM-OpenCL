#include "TestCalculateParticleToFace.h"

#include "TestHelpers.h"

#include <QTest>

TestCalculateParticleToFace::TestCalculateParticleToFace() {}

void TestCalculateParticleToFace::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available");
}

void TestCalculateParticleToFace::cleanupTestCase()
{
  delete this->harness;
}

void TestCalculateParticleToFace::particleAboveFace()
{
  ParticleCL particle = makeParticleCL(0.0f, 0.0f, 1.0f, 0.5f);
  particle.neighborhood.numFaces = 1;

  cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v2 = {5.0f, 5.0f, 0.0f, 0.0f};
  FaceCL face = makeFaceCL(v0, v1, v2);

  MaterialCL mat = makeMaterialCL(3, 10.0f, 2.0f);

  cl_float4 force;
  cl_float4 torque;

  this->harness->runParticleToFaceWorker(particle, face, mat, force, torque);

  QCOMPARE(force.x, 0.0f);
  QCOMPARE(force.y, 0.0f);
  QCOMPARE(force.z, 40.0f);
  QCOMPARE(torque.x, 0.0f);
  QCOMPARE(torque.y, 0.0f);
  QCOMPARE(torque.z, 0.0f);
}

void TestCalculateParticleToFace::beyondThreshold()
{
  ParticleCL particle = makeParticleCL(0.0f, 0.0f, 1.0f, 0.5f);
  particle.neighborhood.numFaces = 1;

  cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v2 = {5.0f, 5.0f, 0.0f, 0.0f};
  FaceCL face = makeFaceCL(v0, v1, v2);

  MaterialCL mat = makeMaterialCL(3, 10.0f, 0.3f);

  cl_float4 force;
  cl_float4 torque;

  this->harness->runParticleToFaceWorker(particle, face, mat, force, torque);

  QCOMPARE(force.x, 0.0f);
  QCOMPARE(force.y, 0.0f);
  QCOMPARE(force.z, 0.0f);
  QCOMPARE(torque.x, 0.0f);
  QCOMPARE(torque.y, 0.0f);
  QCOMPARE(torque.z, 0.0f);
}

void TestCalculateParticleToFace::numFacesDivision()
{
  ParticleCL particle = makeParticleCL(0.0f, 0.0f, 1.0f, 0.5f);
  particle.neighborhood.numFaces = 2;

  cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v2 = {5.0f, 5.0f, 0.0f, 0.0f};
  FaceCL face = makeFaceCL(v0, v1, v2);

  MaterialCL mat = makeMaterialCL(3, 10.0f, 2.0f);

  cl_float4 force;
  cl_float4 torque;

  this->harness->runParticleToFaceWorker(particle, face, mat, force, torque);

  QCOMPARE(force.x, 0.0f);
  QCOMPARE(force.y, 0.0f);
  QCOMPARE(force.z, 20.0f);
  QCOMPARE(torque.x, 0.0f);
  QCOMPARE(torque.y, 0.0f);
  QCOMPARE(torque.z, 0.0f);
}
