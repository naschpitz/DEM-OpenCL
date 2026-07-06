#include "TestEdge.h"

#include "TestHelpers.h"

#include <QTest>
#include <cmath>

TestEdge::TestEdge()
{
  this->harness = nullptr;
}

void TestEdge::initTestCase()
{
  this->harness = new KernelTestHarness();

  if (!this->harness->isReady())
    QSKIP("OpenCL device not available or test kernel compilation failed.");
}

void TestEdge::cleanupTestCase()
{
  delete this->harness;
  this->harness = nullptr;
}

void TestEdge::getClosestToInside()
{
  std::vector<VertexCL> edgeVerts = {makeVertexCL(0.0f, 0.0f, 0.0f), makeVertexCL(10.0f, 0.0f, 0.0f)};

  cl_float4 query = {7.0f, 3.0f, 0.0f, 0.0f};

  cl_float4 result = this->harness->runEdgeGetClosestTo(edgeVerts, {query})[0];

  QCOMPARE(result.x, 7.0f);
  QCOMPARE(result.y, 0.0f);
  QCOMPARE(result.z, 0.0f);
}

void TestEdge::getClosestToBeforeP1()
{
  std::vector<VertexCL> edgeVerts = {makeVertexCL(0.0f, 0.0f, 0.0f), makeVertexCL(10.0f, 0.0f, 0.0f)};

  cl_float4 query = {-2.0f, 5.0f, 0.0f, 0.0f};

  cl_float4 result = this->harness->runEdgeGetClosestTo(edgeVerts, {query})[0];

  QCOMPARE(result.x, 0.0f);
  QCOMPARE(result.y, 0.0f);
  QCOMPARE(result.z, 0.0f);
}

void TestEdge::getClosestToAfterP2()
{
  std::vector<VertexCL> edgeVerts = {makeVertexCL(0.0f, 0.0f, 0.0f), makeVertexCL(10.0f, 0.0f, 0.0f)};

  cl_float4 query = {15.0f, 4.0f, 0.0f, 0.0f};

  cl_float4 result = this->harness->runEdgeGetClosestTo(edgeVerts, {query})[0];

  QCOMPARE(result.x, 10.0f);
  QCOMPARE(result.y, 0.0f);
  QCOMPARE(result.z, 0.0f);
}

void TestEdge::getClosestToCollinearBeyond()
{
  std::vector<VertexCL> edgeVerts = {makeVertexCL(0.0f, 0.0f, 0.0f), makeVertexCL(10.0f, 0.0f, 0.0f)};

  cl_float4 query = {20.0f, 0.0f, 0.0f, 0.0f};

  cl_float4 result = this->harness->runEdgeGetClosestTo(edgeVerts, {query})[0];

  QCOMPARE(result.x, 10.0f);
  QCOMPARE(result.y, 0.0f);
  QCOMPARE(result.z, 0.0f);
}

void TestEdge::getClosestToDegenerate()
{
  std::vector<VertexCL> edgeVerts = {makeVertexCL(5.0f, 5.0f, 0.0f), makeVertexCL(5.0f, 5.0f, 0.0f)};

  cl_float4 query = {8.0f, 9.0f, 0.0f, 0.0f};

  cl_float4 result = this->harness->runEdgeGetClosestTo(edgeVerts, {query})[0];

  QVERIFY(std::isnan(result.x));
  QVERIFY(std::isnan(result.y));
  QVERIFY(std::isnan(result.z));
}
