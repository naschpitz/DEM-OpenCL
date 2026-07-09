#include "TestCalculateFaceToFace.h"

#include "TestHelpers.h"

#include <QTest>

TestCalculateFaceToFace::TestCalculateFaceToFace()
{
  this->harness = nullptr;
}

void TestCalculateFaceToFace::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available or test kernel compilation failed.");
}

void TestCalculateFaceToFace::cleanupTestCase()
{
  delete this->harness;
  this->harness = nullptr;
}

void TestCalculateFaceToFace::hookeForce()
{
  cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v2 = {5.0f, 5.0f, 0.0f, 0.0f};

  FaceCL faceA = makeFaceCL(v0, v1, v2);

  cl_float4 v3 = {-5.0f, -5.0f, 1.0f, 0.0f};
  cl_float4 v4 = {5.0f, -5.0f, 1.0f, 0.0f};
  cl_float4 v5 = {5.0f, 5.0f, 1.0f, 0.0f};

  FaceCL faceB = makeFaceCL(v3, v4, v5);

  std::vector<FaceCL> faces = {faceA, faceB};

  MaterialCL mat = makeMaterialCL(1, 100.0f, 2.0f);
  MaterialsManagerCL mm = makeMaterialsManagerCL(mat);

  this->harness->runCalculateFaceToFace(faces, mm, 0, 1, 1, 1);

  // Hooke's law: force = -k * distance, totalForce = k * distance
  // distance = (0, 0, 1), k = 100 -> totalForce = (0, 0, 100)
  QCOMPARE(faces[0].currentForce.x, 0.0f);
  QCOMPARE(faces[0].currentForce.y, 0.0f);
  QCOMPARE(faces[0].currentForce.z, 100.0f);
}

void TestCalculateFaceToFace::beyondThreshold()
{
  cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v2 = {5.0f, 5.0f, 0.0f, 0.0f};

  FaceCL faceA = makeFaceCL(v0, v1, v2);

  cl_float4 v3 = {-5.0f, -5.0f, 1.0f, 0.0f};
  cl_float4 v4 = {5.0f, -5.0f, 1.0f, 0.0f};
  cl_float4 v5 = {5.0f, 5.0f, 1.0f, 0.0f};

  FaceCL faceB = makeFaceCL(v3, v4, v5);

  std::vector<FaceCL> faces = {faceA, faceB};

  MaterialCL mat = makeMaterialCL(1, 100.0f, 0.5f);
  MaterialsManagerCL mm = makeMaterialsManagerCL(mat);

  this->harness->runCalculateFaceToFace(faces, mm, 0, 1, 1, 1);

  // Distance 1.0 > threshold 0.5 -> no force
  QCOMPARE(faces[0].currentForce.x, 0.0f);
  QCOMPARE(faces[0].currentForce.y, 0.0f);
  QCOMPARE(faces[0].currentForce.z, 0.0f);
}

void TestCalculateFaceToFace::overlapping()
{
  cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v2 = {5.0f, 5.0f, 0.0f, 0.0f};

  FaceCL faceA = makeFaceCL(v0, v1, v2);
  FaceCL faceB = makeFaceCL(v0, v1, v2);

  std::vector<FaceCL> faces = {faceA, faceB};

  MaterialCL mat = makeMaterialCL(1, 100.0f, 2.0f);
  MaterialsManagerCL mm = makeMaterialsManagerCL(mat);

  this->harness->runCalculateFaceToFace(faces, mm, 0, 1, 1, 1);

  // Distance 0 -> force = -k * 0 = 0
  QCOMPARE(faces[0].currentForce.x, 0.0f);
  QCOMPARE(faces[0].currentForce.y, 0.0f);
  QCOMPARE(faces[0].currentForce.z, 0.0f);
}

void TestCalculateFaceToFace::swapped()
{
  cl_float4 v0 = {-5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v1 = {5.0f, -5.0f, 0.0f, 0.0f};
  cl_float4 v2 = {5.0f, 5.0f, 0.0f, 0.0f};

  FaceCL faceA = makeFaceCL(v0, v1, v2);

  cl_float4 v3 = {-5.0f, -5.0f, 1.0f, 0.0f};
  cl_float4 v4 = {5.0f, -5.0f, 1.0f, 0.0f};
  cl_float4 v5 = {5.0f, 5.0f, 1.0f, 0.0f};

  FaceCL faceB = makeFaceCL(v3, v4, v5);

  std::vector<FaceCL> faces = {faceA, faceB};

  MaterialCL mat = makeMaterialCL(1, 100.0f, 2.0f);
  MaterialsManagerCL mm = makeMaterialsManagerCL(mat);

  // Swap: thisFace = B (index 1), otherFace = A (index 0)
  this->harness->runCalculateFaceToFace(faces, mm, 1, 1, 0, 1);

  // distance = closestOnA - closestOnB = (0, 0, -1)
  // force = -k * distance = -100 * (0, 0, -1) = (0, 0, 100)
  // totalForce = -force = (0, 0, -100)
  QCOMPARE(faces[1].currentForce.x, 0.0f);
  QCOMPARE(faces[1].currentForce.y, 0.0f);
  QCOMPARE(faces[1].currentForce.z, -100.0f);
}
