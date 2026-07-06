#include "TestVector.h"

#include <QTest>

TestVector::TestVector()
{
  this->harness = nullptr;
}

void TestVector::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available or test kernel compilation failed.");
}

void TestVector::cleanupTestCase()
{
  delete this->harness;
  this->harness = nullptr;
}

void TestVector::getUnitaryGeneric()
{
  cl_float4 input = {3.0f, 0.0f, 0.0f, 0.0f};

  cl_float4 result = this->harness->runVectorGetUnitary({input})[0];

  QCOMPARE(result.x, 1.0f);
  QCOMPARE(result.y, 0.0f);
  QCOMPARE(result.z, 0.0f);
}

void TestVector::getUnitaryAlreadyUnit()
{
  cl_float4 input = {0.6f, 0.8f, 0.0f, 0.0f};

  cl_float4 result = this->harness->runVectorGetUnitary({input})[0];

  QCOMPARE(result.x, 0.6f);
  QCOMPARE(result.y, 0.8f);
  QCOMPARE(result.z, 0.0f);
}

void TestVector::getUnitaryZero()
{
  cl_float4 input = {0.0f, 0.0f, 0.0f, 0.0f};

  cl_float4 result = this->harness->runVectorGetUnitary({input})[0];

  QCOMPARE(result.x, 0.0f);
  QCOMPARE(result.y, 0.0f);
  QCOMPARE(result.z, 0.0f);
}
