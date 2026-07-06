#include "TestVertex.h"

#include <QTest>

TestVertex::TestVertex() {}

void TestVertex::defaultConstructor()
{
  Vertex v;

  QCOMPARE(v.getCurrentPosition().getX(), 0.0f);
  QCOMPARE(v.getCurrentPosition().getY(), 0.0f);
  QCOMPARE(v.getCurrentPosition().getZ(), 0.0f);

  VertexCL cl = v.getCL();
  QCOMPARE(cl.fixed, static_cast<cl_bool>(CL_FALSE));
}

void TestVertex::xyzConstructor()
{
  Vertex v(1.0, 2.0, 3.0);

  QCOMPARE(v.getCurrentPosition().getX(), 1.0f);
  QCOMPARE(v.getCurrentPosition().getY(), 2.0f);
  QCOMPARE(v.getCurrentPosition().getZ(), 3.0f);

  VertexCL cl = v.getCL();
  QCOMPARE(cl.currentPosition.x, 1.0f);
  QCOMPARE(cl.currentPosition.y, 2.0f);
  QCOMPARE(cl.currentPosition.z, 3.0f);
  QCOMPARE(cl.oldPosition.x, 1.0f);
  QCOMPARE(cl.originalPosition.x, 1.0f);
  QCOMPARE(cl.fixed, static_cast<cl_bool>(CL_FALSE));
}

void TestVertex::vectorConstructor()
{
  Vertex v(Vector3D(4.0f, 5.0f, 6.0f));

  QCOMPARE(v.getCurrentPosition().getX(), 4.0f);
  QCOMPARE(v.getCurrentPosition().getY(), 5.0f);
  QCOMPARE(v.getCurrentPosition().getZ(), 6.0f);

  VertexCL cl = v.getCL();
  QCOMPARE(cl.currentPosition.x, 4.0f);
  QCOMPARE(cl.oldPosition.x, 4.0f);
  QCOMPARE(cl.originalPosition.x, 4.0f);
}

void TestVertex::copyConstructor()
{
  Vertex original(1.0, 2.0, 3.0);
  original.setFixed(true);

  Vertex copy(original);

  QCOMPARE(copy.getCurrentPosition().getX(), 1.0f);
  QCOMPARE(copy.getCurrentPosition().getY(), 2.0f);
  QCOMPARE(copy.getCurrentPosition().getZ(), 3.0f);
  QCOMPARE(copy.getCL().fixed, static_cast<cl_bool>(CL_TRUE));
}

void TestVertex::assignmentOperator()
{
  Vertex original(1.0, 2.0, 3.0);
  original.setFixed(true);

  Vertex assigned;
  assigned = original;

  QCOMPARE(assigned.getCurrentPosition().getX(), 1.0f);
  QCOMPARE(assigned.getCurrentPosition().getY(), 2.0f);
  QCOMPARE(assigned.getCurrentPosition().getZ(), 3.0f);
  QCOMPARE(assigned.getCL().fixed, static_cast<cl_bool>(CL_TRUE));
}

void TestVertex::setFixed()
{
  Vertex v;

  v.setFixed(true);
  QCOMPARE(v.getCL().fixed, static_cast<cl_bool>(CL_TRUE));

  v.setFixed(false);
  QCOMPARE(v.getCL().fixed, static_cast<cl_bool>(CL_FALSE));
}

void TestVertex::setPosition()
{
  Vertex v;

  v.setPosition(Vector3D(7.0f, 8.0f, 9.0f));

  VertexCL cl = v.getCL();
  QCOMPARE(cl.currentPosition.x, 7.0f);
  QCOMPARE(cl.currentPosition.y, 8.0f);
  QCOMPARE(cl.currentPosition.z, 9.0f);
  QCOMPARE(cl.oldPosition.x, 7.0f);
  QCOMPARE(cl.originalPosition.x, 7.0f);
}

void TestVertex::setVelocity()
{
  Vertex v;

  v.setVelocity(Vector3D(1.0f, 2.0f, 3.0f));

  VertexCL cl = v.getCL();
  QCOMPARE(cl.currentVelocity.x, 1.0f);
  QCOMPARE(cl.currentVelocity.y, 2.0f);
  QCOMPARE(cl.currentVelocity.z, 3.0f);
  QCOMPARE(cl.oldVelocity.x, 1.0f);
  QCOMPARE(cl.originalVelocity.x, 1.0f);
}

void TestVertex::displaceBy()
{
  Vertex v(1.0, 1.0, 1.0);

  v.displaceBy(Vector3D(0.5f, 0.5f, 0.5f));

  VertexCL cl = v.getCL();
  QCOMPARE(cl.currentPosition.x, 1.5f);
  QCOMPARE(cl.currentPosition.y, 1.5f);
  QCOMPARE(cl.currentPosition.z, 1.5f);
  QCOMPARE(cl.oldPosition.x, 1.5f);
  QCOMPARE(cl.originalPosition.x, 1.5f);
}

void TestVertex::getCLsetCLRoundtrip()
{
  VertexCL input;
  input.currentPosition = {10.0f, 20.0f, 30.0f, 0.0f};
  input.oldPosition = {40.0f, 50.0f, 60.0f, 0.0f};
  input.originalPosition = {70.0f, 80.0f, 90.0f, 0.0f};
  input.currentVelocity = {1.0f, 2.0f, 3.0f, 0.0f};
  input.oldVelocity = {4.0f, 5.0f, 6.0f, 0.0f};
  input.originalVelocity = {7.0f, 8.0f, 9.0f, 0.0f};
  input.fixed = CL_TRUE;

  Vertex v;
  v.setCL(input);

  VertexCL output = v.getCL();

  QCOMPARE(output.currentPosition.x, 10.0f);
  QCOMPARE(output.currentPosition.y, 20.0f);
  QCOMPARE(output.currentPosition.z, 30.0f);
  QCOMPARE(output.oldPosition.x, 40.0f);
  QCOMPARE(output.originalPosition.x, 70.0f);
  QCOMPARE(output.currentVelocity.x, 1.0f);
  QCOMPARE(output.oldVelocity.x, 4.0f);
  QCOMPARE(output.originalVelocity.x, 7.0f);
}

void TestVertex::getJson()
{
  Vertex v(1.0, 2.0, 3.0);
  v.setVelocity(Vector3D(4.0f, 5.0f, 6.0f));

  nlohmann::json json = v.getJson();

  QVERIFY(json.find("position") != json.end());
  QVERIFY(json.find("velocity") != json.end());

  QCOMPARE(json["position"][0].get<float>(), 1.0f);
  QCOMPARE(json["position"][1].get<float>(), 2.0f);
  QCOMPARE(json["position"][2].get<float>(), 3.0f);

  QCOMPARE(json["velocity"][0].get<float>(), 4.0f);
  QCOMPARE(json["velocity"][1].get<float>(), 5.0f);
  QCOMPARE(json["velocity"][2].get<float>(), 6.0f);
}
