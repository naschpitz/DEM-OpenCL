#include "TestMaterial.h"

#include <QTest>
#include <cmath>

TestMaterial::TestMaterial()
{
  this->harness = nullptr;
}

void TestMaterial::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available or test kernel compilation failed.");
}

void TestMaterial::cleanupTestCase()
{
  delete this->harness;
  this->harness = nullptr;
}

static MaterialCL makeMaterial(int forceType, int dragForceType)
{
  MaterialCL m = {};

  m.forceType = forceType;
  m.dragForceType = dragForceType;
  m.distanceThreshold = 1.0f;

  return m;
}

static MaterialCL makeMaterialWithCoeffs(int forceType, int dragForceType, std::initializer_list<float> coeffs)
{
  MaterialCL m = makeMaterial(forceType, dragForceType);

  int i = 0;
  for (float c : coeffs)
    m.coefficients[i++] = c;

  return m;
}

static MaterialCL makeMaterialWithDragCoeffs(int forceType, int dragForceType, std::initializer_list<float> dragCoeffs)
{
  MaterialCL m = makeMaterial(forceType, dragForceType);

  int i = 0;
  for (float c : dragCoeffs)
    m.dragCoefficients[i++] = c;

  return m;
}

static cl_float4 makeFloat4(float x, float y, float z)
{
  cl_float4 v = {x, y, z, 0.0f};
  return v;
}

void TestMaterial::hooksLaw()
{
  MaterialCL m = makeMaterialWithCoeffs(hooks_law, -1, {100.0f});

  cl_float4 distance = makeFloat4(0.1f, 0.0f, 0.0f);
  cl_float4 unitary = makeFloat4(1.0f, 0.0f, 0.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QCOMPARE(result.x, -10.0f);
  QCOMPARE(result.y, 0.0f);
  QCOMPARE(result.z, 0.0f);
}

void TestMaterial::inverseLinearExternal()
{
  MaterialCL m = makeMaterialWithCoeffs(inverse_linear, -1, {10.0f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 2.0f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.x) < 1e-5f);
  QVERIFY(qAbs(result.y) < 1e-5f);
  QCOMPARE(result.z, 5.0f);
}

void TestMaterial::inverseLinearInternal()
{
  MaterialCL m = makeMaterialWithCoeffs(inverse_linear, -1, {10.0f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 2.0f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, true, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.x) < 1e-5f);
  QVERIFY(qAbs(result.y) < 1e-5f);
  QCOMPARE(result.z, -5.0f);
}

void TestMaterial::inverseQuadratic()
{
  MaterialCL m = makeMaterialWithCoeffs(inverse_quadratic, -1, {10.0f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 2.0f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.z - 2.5f) < 1e-5f);
}

void TestMaterial::inverseCubic()
{
  MaterialCL m = makeMaterialWithCoeffs(inverse_cubic, -1, {10.0f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 2.0f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.z - 1.25f) < 1e-5f);
}

void TestMaterial::adiabaticCompression()
{
  MaterialCL m = makeMaterialWithCoeffs(adiabatic_compression, -1, {1.0f, 1.0f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 2.0f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 2.0f, oldForce);

  QVERIFY(qAbs(result.z - 6.28319f) < 1e-2f);
}

void TestMaterial::morseEquilibrium()
{
  MaterialCL m = makeMaterialWithCoeffs(morse, -1, {1.0f, 2.0f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 0.0f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.x) < 1e-5f);
  QVERIFY(qAbs(result.y) < 1e-5f);
  QVERIFY(qAbs(result.z) < 1e-5f);
}

void TestMaterial::morseNontrivial()
{
  MaterialCL m = makeMaterialWithCoeffs(morse, -1, {1.0f, 2.0f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.z - (-0.46508f)) < 1e-2f);
}

void TestMaterial::lennardJones()
{
  MaterialCL m = makeMaterialWithCoeffs(lennard_jones, -1, {1.0f, 1.0f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 0.5f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.z - (-0.29630f)) < 1e-3f);
}

void TestMaterial::realisticElastic()
{
  MaterialCL m = makeMaterialWithCoeffs(realistic_material, -1, {100.0f, 0.5f, 200.0f, 0.8f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 0.1f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.x) < 1e-5f);
  QVERIFY(qAbs(result.y) < 1e-5f);
  QVERIFY(qAbs(result.z - (-10.0f)) < 1e-4f);
}

void TestMaterial::realisticInternal()
{
  MaterialCL m = makeMaterialWithCoeffs(realistic_material, -1, {100.0f, 0.5f, 200.0f, 0.8f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 0.5f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, true, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.z - (-50.0f)) < 1e-4f);
}

void TestMaterial::realisticPlasticBelowMax()
{
  MaterialCL m = makeMaterialWithCoeffs(realistic_material, -1, {100.0f, 0.2f, 5.0f, 0.8f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 0.5f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 3.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.z - (-3.0f)) < 1e-3f);
}

void TestMaterial::realisticPlasticAboveMax()
{
  MaterialCL m = makeMaterialWithCoeffs(realistic_material, -1, {100.0f, 0.2f, 5.0f, 0.8f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 0.5f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 1.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 10.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.z - 5.0f) < 1e-3f);
}

void TestMaterial::nanGuard()
{
  MaterialCL m = makeMaterialWithCoeffs(inverse_linear, -1, {10.0f});

  cl_float4 distance = makeFloat4(0.0f, 0.0f, 0.0f);
  cl_float4 unitary = makeFloat4(0.0f, 0.0f, 0.0f);
  cl_float4 oldForce = makeFloat4(0.0f, 0.0f, 0.0f);

  cl_float4 result = this->harness->runMaterialCalculateForce(m, distance, unitary, false, 1.0f, 1.0f, oldForce);

  QVERIFY(qAbs(result.x) < 1e-5f);
  QVERIFY(qAbs(result.y) < 1e-5f);
  QVERIFY(qAbs(result.z) < 1e-5f);
}

void TestMaterial::dragLinear()
{
  MaterialCL m = makeMaterialWithDragCoeffs(-1, linear, {0.5f});

  cl_float4 velocity = makeFloat4(1.0f, 0.0f, 0.0f);
  cl_float4 rotation = makeFloat4(0.0f, 0.0f, 0.0f);
  cl_float4 force = makeFloat4(0.0f, 0.0f, 3.0f);

  cl_float4 result = this->harness->runMaterialCalculateDragForce(m, velocity, rotation, force);

  QCOMPARE(result.x, -1.5f);
  QVERIFY(qAbs(result.y) < 1e-5f);
  QVERIFY(qAbs(result.z) < 1e-5f);
}

void TestMaterial::dragLinearWithRotation()
{
  MaterialCL m = makeMaterialWithDragCoeffs(-1, linear, {0.5f});

  cl_float4 velocity = makeFloat4(1.0f, 0.0f, 0.0f);
  cl_float4 rotation = makeFloat4(0.0f, 1.0f, 0.0f);
  cl_float4 force = makeFloat4(0.0f, 0.0f, 2.0f);

  cl_float4 result = this->harness->runMaterialCalculateDragForce(m, velocity, rotation, force);

  QCOMPARE(result.x, -1.0f);
  QCOMPARE(result.y, -1.0f);
  QVERIFY(qAbs(result.z) < 1e-5f);
}

void TestMaterial::dragQuadratic()
{
  MaterialCL m = makeMaterialWithDragCoeffs(-1, quadratic, {1.0f});

  cl_float4 velocity = makeFloat4(2.0f, 0.0f, 0.0f);
  cl_float4 rotation = makeFloat4(0.0f, 0.0f, 0.0f);
  cl_float4 force = makeFloat4(0.0f, 0.0f, 4.0f);

  cl_float4 result = this->harness->runMaterialCalculateDragForce(m, velocity, rotation, force);

  QCOMPARE(result.x, -16.0f);
  QVERIFY(qAbs(result.y) < 1e-5f);
  QVERIFY(qAbs(result.z) < 1e-5f);
}

void TestMaterial::dragCubic()
{
  MaterialCL m = makeMaterialWithDragCoeffs(-1, cubic, {1.0f});

  cl_float4 velocity = makeFloat4(2.0f, 0.0f, 0.0f);
  cl_float4 rotation = makeFloat4(0.0f, 0.0f, 0.0f);
  cl_float4 force = makeFloat4(0.0f, 0.0f, 2.0f);

  cl_float4 result = this->harness->runMaterialCalculateDragForce(m, velocity, rotation, force);

  QCOMPARE(result.x, -16.0f);
  QVERIFY(qAbs(result.y) < 1e-5f);
  QVERIFY(qAbs(result.z) < 1e-5f);
}

void TestMaterial::dragNone()
{
  MaterialCL m = makeMaterialWithDragCoeffs(-1, -1, {0.5f});

  cl_float4 velocity = makeFloat4(1.0f, 0.0f, 0.0f);
  cl_float4 rotation = makeFloat4(0.0f, 0.0f, 0.0f);
  cl_float4 force = makeFloat4(0.0f, 0.0f, 5.0f);

  cl_float4 result = this->harness->runMaterialCalculateDragForce(m, velocity, rotation, force);

  QVERIFY(qAbs(result.x) < 1e-5f);
  QVERIFY(qAbs(result.y) < 1e-5f);
  QVERIFY(qAbs(result.z) < 1e-5f);
}
