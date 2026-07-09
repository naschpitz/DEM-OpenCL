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

TestCalculateFaceToFace::PairResult TestCalculateFaceToFace::runPair(const cl_float4& a0, const cl_float4& a1,
                                                                     const cl_float4& a2, const cl_float4& b0,
                                                                     const cl_float4& b1, const cl_float4& b2,
                                                                     float stiffness, float threshold)
{
  FaceCL faceA = makeFaceCL(a0, a1, a2);
  FaceCL faceB = makeFaceCL(b0, b1, b2);

  std::vector<FaceCL> faces = {faceA, faceB};

  // forceType 1 = hooks_law -> material_calculateForce returns -stiffness*distance, and
  // faceToFaceWorker_run accumulates -(force+drag) = stiffness*distance on thisFace. drag is
  // zero (dragForceType = -1). So currentForce == stiffness * (closestOnOther - closestOnThis):
  // an exact readout of the distance vector produced by face_getClosestToFace.
  MaterialCL mat = makeMaterialCL(1 /*hooks_law*/, stiffness, threshold);
  MaterialsManagerCL mm = makeMaterialsManagerCL(mat);

  this->harness->runCalculateFaceToFace(faces, mm);

  return {faces[0].currentForce, faces[1].currentForce};
}

void TestCalculateFaceToFace::verifyForce(const cl_float4& force, float ex, float ey, float ez)
{
  // face_getClosestToFace is an iterative solver that converges to ~1e-6 in *distance*, which
  // leaves ~1e-7 residuals in the closest-point *coordinates* (larger when a barycentric/edge
  // parameter is non-dyadic, e.g. t = 1/3). Scaled by the Hooke stiffness (100 here), those
  // show up as ~1e-5 residuals on force components that are mathematically exact integers or
  // zero. QCOMPARE's fuzzy compare rejects a value-vs-zero comparison, so use a relative
  // tolerance with an absolute floor: tight enough that any wrong branch (which moves a force
  // component by O(100)) fails, loose enough to absorb the iterative residual across vendors.
  auto close = [](float actual, float expected) {
    return qAbs(actual - expected) <= 1e-3f * qAbs(expected) + 1e-2f;
  };

  QVERIFY(close(force.x, ex));
  QVERIFY(close(force.y, ey));
  QVERIFY(close(force.z, ez));
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

  this->harness->runCalculateFaceToFace(faces, mm);

  // Hooke's law: force = -k * distance, totalForce = k * distance
  // Face A: distance = (0, 0, 1), totalForce = (0, 0, 100)
  // Face B: distance = (0, 0, -1), totalForce = (0, 0, -100)
  QCOMPARE(faces[0].currentForce.x, 0.0f);
  QCOMPARE(faces[0].currentForce.y, 0.0f);
  QCOMPARE(faces[0].currentForce.z, 100.0f);
  QCOMPARE(faces[1].currentForce.x, 0.0f);
  QCOMPARE(faces[1].currentForce.y, 0.0f);
  QCOMPARE(faces[1].currentForce.z, -100.0f);
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

  this->harness->runCalculateFaceToFace(faces, mm);

  // Distance 1.0 > threshold 0.5 -> no force on either face
  QCOMPARE(faces[0].currentForce.x, 0.0f);
  QCOMPARE(faces[0].currentForce.y, 0.0f);
  QCOMPARE(faces[0].currentForce.z, 0.0f);
  QCOMPARE(faces[1].currentForce.x, 0.0f);
  QCOMPARE(faces[1].currentForce.y, 0.0f);
  QCOMPARE(faces[1].currentForce.z, 0.0f);
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

  this->harness->runCalculateFaceToFace(faces, mm);

  // Distance 0 -> force = -k * 0 = 0 on both faces
  QCOMPARE(faces[0].currentForce.x, 0.0f);
  QCOMPARE(faces[0].currentForce.y, 0.0f);
  QCOMPARE(faces[0].currentForce.z, 0.0f);
  QCOMPARE(faces[1].currentForce.x, 0.0f);
  QCOMPARE(faces[1].currentForce.y, 0.0f);
  QCOMPARE(faces[1].currentForce.z, 0.0f);
}

// Reference face A used by most geometry cases: a right triangle in the z=0 plane with the
// right angle at the origin, legs of length 10 along +x and +y, hypotenuse x+y=10.
//   v0 = (0,0,0)  v1 = (10,0,0)  v2 = (0,10,0)
// Edge e1 = v0-v1 (x-axis), e2 = v0-v2 (y-axis), e3 = v1-v2 (hypotenuse).

void TestCalculateFaceToFace::vertexToVertexDiagonal()
{
  // A's corner v0 and B's corner b0 are the unique mutual closest pair, with a fully 3D
  // (non-axis-aligned) diagonal separating them. Exercises the vertex clamp of edge_getClosestTo
  // on both faces.
  //   cA = (0,0,0)   cB = (-3,-3,4)   distance = (-3,-3,4)
  cl_float4 a0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a2 = {0.0f, 10.0f, 0.0f, 0.0f};

  cl_float4 b0 = {-3.0f, -3.0f, 4.0f, 0.0f};
  cl_float4 b1 = {-8.0f, -3.0f, 4.0f, 0.0f};
  cl_float4 b2 = {-3.0f, -8.0f, 4.0f, 0.0f};

  auto r = runPair(a0, a1, a2, b0, b1, b2, 100.0f, 100.0f);

  verifyForce(r.forceA, -300.0f, -300.0f, 400.0f);
  verifyForce(r.forceB, 300.0f, 300.0f, -400.0f);
}

void TestCalculateFaceToFace::closestOnEdgeVNegative()
{
  // B's nearest corner projects past A's edge v0-v1 (barycentric v < 0), so face_projectPoint
  // takes the e1 branch and edge_getClosestTo clamps to the edge interior at t = 0.5.
  //   cA = (5,0,0)  (interior of A's e1)   cB = (5,-2,4)   distance = (0,-2,4)
  cl_float4 a0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a2 = {0.0f, 10.0f, 0.0f, 0.0f};

  cl_float4 b0 = {5.0f, -2.0f, 4.0f, 0.0f};
  cl_float4 b1 = {5.0f, -5.0f, 4.0f, 0.0f};
  cl_float4 b2 = {2.0f, -2.0f, 4.0f, 0.0f};

  auto r = runPair(a0, a1, a2, b0, b1, b2, 100.0f, 100.0f);

  verifyForce(r.forceA, 0.0f, -200.0f, 400.0f);
  verifyForce(r.forceB, 0.0f, 200.0f, -400.0f);
}

void TestCalculateFaceToFace::closestOnEdgeUNegative()
{
  // B's nearest corner projects past A's edge v0-v2 (barycentric u < 0), taking the e2 branch
  // and clamping to the edge interior at t = 0.5. Mirrors the v<0 case on the other leg.
  //   cA = (0,5,0)  (interior of A's e2)   cB = (-2,5,4)   distance = (-2,0,4)
  cl_float4 a0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a2 = {0.0f, 10.0f, 0.0f, 0.0f};

  cl_float4 b0 = {-2.0f, 5.0f, 4.0f, 0.0f};
  cl_float4 b1 = {-5.0f, 5.0f, 4.0f, 0.0f};
  cl_float4 b2 = {-2.0f, 2.0f, 4.0f, 0.0f};

  auto r = runPair(a0, a1, a2, b0, b1, b2, 100.0f, 100.0f);

  verifyForce(r.forceA, -200.0f, 0.0f, 400.0f);
  verifyForce(r.forceB, 200.0f, 0.0f, -400.0f);
}

void TestCalculateFaceToFace::closestOnEdgeUVOverflow()
{
  // B's nearest corner projects past A's hypotenuse (barycentric u + v > 1), taking the e3
  // branch and clamping to the hypotenuse midpoint at t = 0.5.
  //   cA = (5,5,0)  (midpoint of A's e3)   cB = (8,8,4)   distance = (3,3,4)
  cl_float4 a0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a2 = {0.0f, 10.0f, 0.0f, 0.0f};

  cl_float4 b0 = {8.0f, 8.0f, 4.0f, 0.0f};
  cl_float4 b1 = {11.0f, 8.0f, 4.0f, 0.0f};
  cl_float4 b2 = {8.0f, 11.0f, 4.0f, 0.0f};

  auto r = runPair(a0, a1, a2, b0, b1, b2, 100.0f, 100.0f);

  verifyForce(r.forceA, 300.0f, 300.0f, 400.0f);
  verifyForce(r.forceB, -300.0f, -300.0f, -400.0f);
}

void TestCalculateFaceToFace::perpendicularFaces()
{
  // A lies in z=0 (normal +z); B lies in x=-1 (normal +x) -> face normals are 90 deg apart.
  // B's nearest corner is offset in y so the closest point on A lands in the interior of edge
  // e2 (the y-axis leg) at y=2, while B's corner is its own nearest point. This proves the
  // solver handles non-parallel face orientations and a horizontal (non-vertical) distance.
  //   cA = (0,2,0)  (interior of A's e2)   cB = (-1,2,0)   distance = (-1,0,0)
  cl_float4 a0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a2 = {0.0f, 10.0f, 0.0f, 0.0f};

  cl_float4 b0 = {-1.0f, 2.0f, 0.0f, 0.0f};
  cl_float4 b1 = {-1.0f, 8.0f, 0.0f, 0.0f};
  cl_float4 b2 = {-1.0f, 2.0f, 6.0f, 0.0f};

  auto r = runPair(a0, a1, a2, b0, b1, b2, 100.0f, 100.0f);

  verifyForce(r.forceA, -100.0f, 0.0f, 0.0f);
  verifyForce(r.forceB, 100.0f, 0.0f, 0.0f);
}

void TestCalculateFaceToFace::tiltedFaces()
{
  // B's plane is non-axis-aligned (a tilted triangle), with its corner b0 the unique nearest
  // point to A. A's nearest point to b0 is its vertex v1, reached via the e1 branch clamping to
  // the edge endpoint. Exercises a fully general orientation plus an edge-endpoint (vertex)
  // clamp on A and an edge clamp on B.
  //   cA = (10,0,0) (A's vertex v1)   cB = (12,-2,3)   distance = (2,-2,3)
  cl_float4 a0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a2 = {0.0f, 10.0f, 0.0f, 0.0f};

  cl_float4 b0 = {12.0f, -2.0f, 3.0f, 0.0f};
  cl_float4 b1 = {15.0f, -2.0f, 6.0f, 0.0f};
  cl_float4 b2 = {12.0f, -5.0f, 6.0f, 0.0f};

  auto r = runPair(a0, a1, a2, b0, b1, b2, 100.0f, 100.0f);

  verifyForce(r.forceA, 200.0f, -200.0f, 300.0f);
  verifyForce(r.forceB, -200.0f, 200.0f, -300.0f);
}

void TestCalculateFaceToFace::coplanarSideBySide()
{
  // Two coplanar (z=0) triangles separated horizontally. The closest pair is A's vertex v1 to
  // B's corner b0, reached via the e3 branch on A (u+v > 1, clamped to the v1 endpoint) and the
  // e2 branch on B. Distance is purely in-plane along +x.
  //   cA = (10,0,0) (A's vertex v1)   cB = (13,0,0)   distance = (3,0,0)
  cl_float4 a0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a2 = {0.0f, 10.0f, 0.0f, 0.0f};

  cl_float4 b0 = {13.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 b1 = {20.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 b2 = {13.0f, 7.0f, 0.0f, 0.0f};

  auto r = runPair(a0, a1, a2, b0, b1, b2, 100.0f, 100.0f);

  verifyForce(r.forceA, 300.0f, 0.0f, 0.0f);
  verifyForce(r.forceB, -300.0f, 0.0f, 0.0f);
}

void TestCalculateFaceToFace::thresholdExact()
{
  // Parallel overlapping faces, gap = 1.0, threshold set exactly to 1.0. The worker test is
  // `length(distance) > threshold` (strict), so equality must still apply the force.
  cl_float4 a0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a2 = {0.0f, 10.0f, 0.0f, 0.0f};

  cl_float4 b0 = {0.0f, 0.0f, 1.0f, 0.0f};
  cl_float4 b1 = {10.0f, 0.0f, 1.0f, 0.0f};
  cl_float4 b2 = {0.0f, 10.0f, 1.0f, 0.0f};

  auto r = runPair(a0, a1, a2, b0, b1, b2, 100.0f, 1.0f);

  verifyForce(r.forceA, 0.0f, 0.0f, 100.0f);
  verifyForce(r.forceB, 0.0f, 0.0f, -100.0f);
}

void TestCalculateFaceToFace::thresholdJustBelow()
{
  // Same geometry, threshold 0.9 < gap 1.0 -> length(distance) > threshold -> early return, no
  // force. Pins the boundary behavior on the other side of the comparison.
  cl_float4 a0 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a1 = {10.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 a2 = {0.0f, 10.0f, 0.0f, 0.0f};

  cl_float4 b0 = {0.0f, 0.0f, 1.0f, 0.0f};
  cl_float4 b1 = {10.0f, 0.0f, 1.0f, 0.0f};
  cl_float4 b2 = {0.0f, 10.0f, 1.0f, 0.0f};

  auto r = runPair(a0, a1, a2, b0, b1, b2, 100.0f, 0.9f);

  verifyForce(r.forceA, 0.0f, 0.0f, 0.0f);
  verifyForce(r.forceB, 0.0f, 0.0f, 0.0f);
}
