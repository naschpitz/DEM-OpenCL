#ifndef TESTCALCULATEFACETOFACE_H
#define TESTCALCULATEFACETOFACE_H

#include <QObject>

#include "KernelTestHarness.h"

class TestCalculateFaceToFace : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

    struct PairResult {
        cl_float4 forceA;
        cl_float4 forceB;
    };

    // Builds two faces, runs the real calculate_face_to_face kernel (Hooke's law), and
    // returns the accumulated currentForce on each. Because forceType = hooks_law makes
    // forceOnFace = stiffness * (closestOnOther - closestOnThis), the returned vectors are
    // a direct readout of the closest-point distance vector computed by face_getClosestToFace.
    PairResult runPair(const cl_float4& a0, const cl_float4& a1, const cl_float4& a2, const cl_float4& b0,
                       const cl_float4& b1, const cl_float4& b2, float stiffness, float threshold);

    void verifyForce(const cl_float4& force, float ex, float ey, float ez);

  public:
    TestCalculateFaceToFace();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    // Arithmetic / threshold sanity (the original three).
    void hookeForce();
    void beyondThreshold();
    void overlapping();

    // Closest-point region coverage through the face-to-face path. Each case below pins the
    // mutual closest pair to a known (exact, dyadic) point pair so the golden force is an
    // exact integer vector, while exercising a distinct branch of face_projectPoint +
    // edge_getClosestTo through the iterative face_getClosestToFace solver.
    void vertexToVertexDiagonal(); // corner-to-corner, fully 3D diagonal distance
    void closestOnEdgeVNegative(); // projection past edge v0-v1 (barycentric v < 0)
    void closestOnEdgeUNegative(); // projection past edge v0-v2 (barycentric u < 0)
    void closestOnEdgeUVOverflow(); // projection past edge v1-v2 (barycentric u + v > 1)
    void perpendicularFaces(); // face normals at 90 deg, vertex-to-edge closest pair
    void tiltedFaces(); // non-axis-aligned (angled) face orientation
    void coplanarSideBySide(); // coplanar triangles, separated horizontally (edge-to-edge)

    // Threshold comparison boundary (the worker uses a strict '>').
    void thresholdExact(); // length(distance) == threshold -> force still applied
    void thresholdJustBelow(); // length(distance) > threshold -> no force
};

#endif // TESTCALCULATEFACETOFACE_H
