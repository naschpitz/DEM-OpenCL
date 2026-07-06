#include "TestFace.h"

#include <QTest>

TestFace::TestFace() {}

void TestFace::constructor()
{
  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(1.0, 0.0, 0.0);
  Vertex v3(0.0, 1.0, 0.0);

  Face face(v1, v2, v3);

  QCOMPARE(face.getVertexes().size(), 3);

  QCOMPARE(face.getVertexes()[0].getCurrentPosition().getX(), 0.0f);
  QCOMPARE(face.getVertexes()[1].getCurrentPosition().getX(), 1.0f);
  QCOMPARE(face.getVertexes()[2].getCurrentPosition().getY(), 1.0f);
}

void TestFace::getArea()
{
  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(1.0, 0.0, 0.0);
  Vertex v3(0.0, 1.0, 0.0);

  Face unitTriangle(v1, v2, v3);

  QCOMPARE(unitTriangle.getArea(), 0.5);

  Vertex w1(0.0, 0.0, 0.0);
  Vertex w2(2.0, 0.0, 0.0);
  Vertex w3(0.0, 0.0, 3.0);

  Face scaledTriangle(w1, w2, w3);

  QCOMPARE(scaledTriangle.getArea(), 3.0);

  Vertex u1(1.0, 0.0, 0.0);
  Vertex u2(0.0, 1.0, 0.0);
  Vertex u3(0.0, 0.0, 1.0);

  Face obliqueTriangle(u1, u2, u3);

  QVERIFY(qAbs(obliqueTriangle.getArea() - 0.8660254037844386) < 1e-6);
}

void TestFace::getNormal()
{
  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(1.0, 0.0, 0.0);
  Vertex v3(0.0, 1.0, 0.0);

  Face face(v1, v2, v3);

  Vector3D normal = face.getNormal();

  QCOMPARE(normal.getX(), 0.0f);
  QCOMPARE(normal.getY(), 0.0f);
  QCOMPARE(normal.getZ(), 1.0f);
}

void TestFace::getCL()
{
  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(1.0, 0.0, 0.0);
  Vertex v3(0.0, 1.0, 0.0);

  Face face(v1, v2, v3);
  face.setMass(10.0);

  FaceCL cl = face.getCL(5, 7);

  QCOMPARE(cl.index, 5u);
  QCOMPARE(cl.materialIndex, 7u);
  QCOMPARE(cl.area, 0.5f);
  QCOMPARE(cl.mass, 10.0f);

  QCOMPARE(cl.vertexes[0].currentPosition.x, 0.0f);
  QCOMPARE(cl.vertexes[1].currentPosition.x, 1.0f);
  QCOMPARE(cl.vertexes[2].currentPosition.y, 1.0f);
}

void TestFace::setCLgetCLRoundtrip()
{
  FaceCL input;
  input.currentPosition = {0.1f, 0.2f, 0.3f, 0.0f};
  input.currentVelocity = {0.4f, 0.5f, 0.6f, 0.0f};
  input.meanVelocity = {0.7f, 0.8f, 0.9f, 0.0f};
  input.currentForce = {1.0f, 2.0f, 3.0f, 0.0f};
  input.meanForce = {4.0f, 5.0f, 6.0f, 0.0f};
  input.oldForce = {7.0f, 8.0f, 9.0f, 0.0f};
  input.currentTorque = {10.0f, 11.0f, 12.0f, 0.0f};
  input.meanTorque = {13.0f, 14.0f, 15.0f, 0.0f};
  input.oldTorque = {16.0f, 17.0f, 18.0f, 0.0f};

  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(0.0, 0.0, 0.0);
  Vertex v3(0.0, 0.0, 0.0);

  Face face(v1, v2, v3);
  face.setCL(input);

  FaceCL output = face.getCL(0, 0);

  QCOMPARE(output.currentPosition.x, 0.1f);
  QCOMPARE(output.currentPosition.y, 0.2f);
  QCOMPARE(output.currentPosition.z, 0.3f);

  QCOMPARE(output.currentForce.x, 1.0f);
  QCOMPARE(output.currentForce.y, 2.0f);
  QCOMPARE(output.currentForce.z, 3.0f);

  QCOMPARE(output.meanForce.x, 4.0f);
  QCOMPARE(output.meanForce.y, 5.0f);
  QCOMPARE(output.meanForce.z, 6.0f);

  QCOMPARE(output.oldForce.x, 7.0f);
  QCOMPARE(output.oldForce.y, 8.0f);
  QCOMPARE(output.oldForce.z, 9.0f);

  QCOMPARE(output.currentTorque.x, 10.0f);
  QCOMPARE(output.currentTorque.y, 11.0f);
  QCOMPARE(output.currentTorque.z, 12.0f);

  QCOMPARE(output.meanTorque.x, 13.0f);
  QCOMPARE(output.meanTorque.y, 14.0f);
  QCOMPARE(output.meanTorque.z, 15.0f);

  QCOMPARE(output.oldTorque.x, 16.0f);
  QCOMPARE(output.oldTorque.y, 17.0f);
  QCOMPARE(output.oldTorque.z, 18.0f);
}

void TestFace::getJson()
{
  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(1.0, 0.0, 0.0);
  Vertex v3(0.0, 1.0, 0.0);

  Face face(v1, v2, v3);

  FaceCL faceCL;
  faceCL.currentPosition = {0.5f, 0.5f, 0.5f, 0.0f};
  faceCL.meanVelocity = {1.0f, 2.0f, 3.0f, 0.0f};
  face.setCL(faceCL);

  nlohmann::json json = face.getJson();

  QVERIFY(json.find("position") != json.end());
  QVERIFY(json.find("velocity") != json.end());
  QVERIFY(json.find("force") != json.end());
  QVERIFY(json.find("vertexes") != json.end());
  QCOMPARE(json["vertexes"].size(), static_cast<size_t>(3));

  QCOMPARE(json["position"][0].get<float>(), 0.5f);
  QCOMPARE(json["position"][1].get<float>(), 0.5f);
  QCOMPARE(json["position"][2].get<float>(), 0.5f);

  QCOMPARE(json["velocity"][0].get<float>(), 1.0f);
  QCOMPARE(json["velocity"][1].get<float>(), 2.0f);
  QCOMPARE(json["velocity"][2].get<float>(), 3.0f);
}

void TestFace::displaceBy()
{
  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(1.0, 0.0, 0.0);
  Vertex v3(0.0, 1.0, 0.0);

  Face face(v1, v2, v3);

  face.displaceBy(Vector3D(1.0f, 2.0f, 3.0f));

  const QVector<Vertex>& vertexes = face.getVertexes();

  QCOMPARE(vertexes[0].getCurrentPosition().getX(), 1.0f);
  QCOMPARE(vertexes[0].getCurrentPosition().getY(), 2.0f);
  QCOMPARE(vertexes[0].getCurrentPosition().getZ(), 3.0f);

  QCOMPARE(vertexes[1].getCurrentPosition().getX(), 2.0f);
  QCOMPARE(vertexes[2].getCurrentPosition().getY(), 3.0f);
}

void TestFace::setFixed()
{
  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(1.0, 0.0, 0.0);
  Vertex v3(0.0, 1.0, 0.0);

  Face face(v1, v2, v3);

  face.setFixed(true);

  FaceCL cl = face.getCL(0, 0);

  QCOMPARE(cl.vertexes[0].fixed, static_cast<cl_bool>(CL_TRUE));
  QCOMPARE(cl.vertexes[1].fixed, static_cast<cl_bool>(CL_TRUE));
  QCOMPARE(cl.vertexes[2].fixed, static_cast<cl_bool>(CL_TRUE));
}

void TestFace::setVelocity()
{
  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(1.0, 0.0, 0.0);
  Vertex v3(0.0, 1.0, 0.0);

  Face face(v1, v2, v3);

  face.setVelocity(Vector3D(1.0f, 2.0f, 3.0f));

  QCOMPARE(face.getCurrentVelocity().getX(), 1.0f);
  QCOMPARE(face.getCurrentVelocity().getY(), 2.0f);
  QCOMPARE(face.getCurrentVelocity().getZ(), 3.0f);

  const QVector<Vertex>& vertexes = face.getVertexes();

  QCOMPARE(vertexes[0].getCurrentVelocity().getX(), 1.0f);
  QCOMPARE(vertexes[1].getCurrentVelocity().getY(), 2.0f);
  QCOMPARE(vertexes[2].getCurrentVelocity().getZ(), 3.0f);
}

void TestFace::getCurrentMomentum()
{
  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(1.0, 0.0, 0.0);
  Vertex v3(0.0, 1.0, 0.0);

  Face face(v1, v2, v3);
  face.setMass(10.0);
  face.setVelocity(Vector3D(1.0f, 0.0f, 0.0f));

  Vector3D momentum = face.getCurrentMomentum();

  QCOMPARE(momentum.getX(), 10.0f);
  QCOMPARE(momentum.getY(), 0.0f);
  QCOMPARE(momentum.getZ(), 0.0f);
}

void TestFace::getCurrentKineticEnergy()
{
  Vertex v1(0.0, 0.0, 0.0);
  Vertex v2(1.0, 0.0, 0.0);
  Vertex v3(0.0, 1.0, 0.0);

  Face face(v1, v2, v3);
  face.setMass(20.0);
  face.setVelocity(Vector3D(3.0f, 4.0f, 0.0f));

  double ke = face.getCurrentKineticEnergy();

  QVERIFY(qAbs(ke - 250.0) < 1e-3);
}
