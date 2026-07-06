#include "TestParticleWorker.h"
#include "TestHelpers.h"

#include <QTest>

static MaterialCL makeWorkerMaterial(int forceType, float coeff, float distanceThreshold)
{
    MaterialCL m{};
    m.materialIndex1 = 0;
    m.materialIndex2 = 0;
    m.forceType = forceType;
    m.dragForceType = -1;
    m.distanceThreshold = distanceThreshold;
    m.coefficients[0] = coeff;
    return m;
}

TestParticleWorker::TestParticleWorker()
{
}

void TestParticleWorker::initTestCase()
{
    this->harness = new KernelTestHarness();

    if(!this->harness->isReady())
        QSKIP("OpenCL device not available");
}

void TestParticleWorker::cleanupTestCase()
{
    delete this->harness;
}

void TestParticleWorker::p2pOverlappingRepel()
{
    ParticleCL thisP  = makeParticleCL(0.0f, 0.0f, 0.0f, 0.5f);
    ParticleCL otherP = makeParticleCL(0.3f, 0.0f, 0.0f, 0.5f);

    MaterialCL mat = makeWorkerMaterial(1, 100.0f, 2.0f);

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

void TestParticleWorker::p2pSeparatedAttract()
{
    ParticleCL thisP  = makeParticleCL(0.0f, 0.0f, 0.0f, 0.5f);
    ParticleCL otherP = makeParticleCL(2.0f, 0.0f, 0.0f, 0.5f);

    MaterialCL mat = makeWorkerMaterial(1, 100.0f, 2.0f);

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

void TestParticleWorker::p2pBeyondThreshold()
{
    ParticleCL thisP  = makeParticleCL(0.0f, 0.0f, 0.0f, 0.5f);
    ParticleCL otherP = makeParticleCL(2.0f, 0.0f, 0.0f, 0.5f);

    MaterialCL mat = makeWorkerMaterial(1, 100.0f, 0.5f);

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

void TestParticleWorker::p2fParticleAboveFace()
{
    ParticleCL particle = makeParticleCL(0.0f, 0.0f, 1.0f, 0.5f);
    particle.neighborhood.numFaces = 1;

    cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
    cl_float4 v1 = { 5.0f, -5.0f, 0.0f, 0.0f};
    cl_float4 v2 = { 5.0f,  5.0f, 0.0f, 0.0f};
    FaceCL face = makeFaceCL(v0, v1, v2);

    MaterialCL mat = makeWorkerMaterial(3, 10.0f, 2.0f);

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

void TestParticleWorker::p2fBeyondThreshold()
{
    ParticleCL particle = makeParticleCL(0.0f, 0.0f, 1.0f, 0.5f);
    particle.neighborhood.numFaces = 1;

    cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
    cl_float4 v1 = { 5.0f, -5.0f, 0.0f, 0.0f};
    cl_float4 v2 = { 5.0f,  5.0f, 0.0f, 0.0f};
    FaceCL face = makeFaceCL(v0, v1, v2);

    MaterialCL mat = makeWorkerMaterial(3, 10.0f, 0.3f);

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

void TestParticleWorker::p2fNumFacesDivision()
{
    ParticleCL particle = makeParticleCL(0.0f, 0.0f, 1.0f, 0.5f);
    particle.neighborhood.numFaces = 2;

    cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
    cl_float4 v1 = { 5.0f, -5.0f, 0.0f, 0.0f};
    cl_float4 v2 = { 5.0f,  5.0f, 0.0f, 0.0f};
    FaceCL face = makeFaceCL(v0, v1, v2);

    MaterialCL mat = makeWorkerMaterial(3, 10.0f, 2.0f);

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
