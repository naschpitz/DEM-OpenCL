#include "TestFace.h"

#include "TestHelpers.h"

#include <QTest>

#include <cmath>

TestFace::TestFace()
{
  this->harness = nullptr;
}

void TestFace::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available or test kernel compilation failed.");
}

void TestFace::cleanupTestCase()
{
  delete this->harness;
  this->harness = nullptr;
}

void TestFace::getClosestToInside()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(2.0f, 2.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 2.0f);
  QCOMPARE(outFace[0].y, 2.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToEdgeVNegative()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(5.0f, -3.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 5.0f);
  QCOMPARE(outFace[0].y, 0.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToEdgeUNegative()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(-3.0f, 5.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 0.0f);
  QCOMPARE(outFace[0].y, 5.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToEdgeUVOverflow()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(8.0f, 8.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 5.0f);
  QCOMPARE(outFace[0].y, 5.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToSeamVZero()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(5.0f, 0.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 5.0f);
  QCOMPARE(outFace[0].y, 0.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToSeamUZero()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(0.0f, 5.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 0.0f);
  QCOMPARE(outFace[0].y, 5.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToSeamUVSumOne()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(5.0f, 5.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 5.0f);
  QCOMPARE(outFace[0].y, 5.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToNearVertexV0()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(-1.0f, -1.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 0.0f);
  QCOMPARE(outFace[0].y, 0.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToNearVertexV1()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(12.0f, -1.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 10.0f);
  QCOMPARE(outFace[0].y, 0.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToNearVertexV2()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(-1.0f, 12.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 0.0f);
  QCOMPARE(outFace[0].y, 10.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToCoplanar()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(2.0f, 2.0f, 0.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 2.0f);
  QCOMPARE(outFace[0].y, 2.0f);
  QCOMPARE(outFace[0].z, 0.0f);
}

void TestFace::getClosestToDegenerate()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {10.0f, 0.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(5.0f, 5.0f, 1.0f, 0.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QVERIFY(std::isnan(outFace[0].x));
  QVERIFY(std::isnan(outFace[0].y));
  QVERIFY(std::isnan(outFace[0].z));
}

void TestFace::getClosestToRadiusOffset()
{
  cl_float4 v0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 v2 = {0.0f, 10.0f, 0.0f, 0.0f};

  std::vector<FaceCL> faces = {makeFaceCL(v0, v1, v2)};
  std::vector<ParticleCL> particles = {makeParticleCL(2.0f, 2.0f, 2.0f, 1.0f)};

  std::vector<cl_float4> outFace;
  std::vector<cl_float4> outParticle;

  this->harness->runFaceGetClosestTo(faces, particles, outFace, outParticle);

  QCOMPARE(outFace[0].x, 2.0f);
  QCOMPARE(outFace[0].y, 2.0f);
  QCOMPARE(outFace[0].z, 0.0f);

  QCOMPARE(outParticle[0].x, 2.0f);
  QCOMPARE(outParticle[0].y, 2.0f);
  QCOMPARE(outParticle[0].z, 1.0f);
}
