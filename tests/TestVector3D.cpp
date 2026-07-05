#include "TestVector3D.h"

#include <QTest>
#include <cmath>

TestVector3D::TestVector3D()
{

}

void TestVector3D::emptyConstructor()
{
    Vector3D vector3d;

    QCOMPARE(vector3d.getX(), 0.0f);
    QCOMPARE(vector3d.getY(), 0.0f);
    QCOMPARE(vector3d.getZ(), 0.0f);
}

void TestVector3D::xyzConstructor()
{
    Vector3D vector3d(1.2f, 3.4f, 5.6f);

    QCOMPARE(vector3d.getX(), 1.2f);
    QCOMPARE(vector3d.getY(), 3.4f);
    QCOMPARE(vector3d.getZ(), 5.6f);
}

void TestVector3D::assignmentOperatorToVector3d()
{
    Vector3D vector3d(1.2f, 3.4f, 5.6f);
    Vector3D otherVector3D = vector3d;

    QCOMPARE(otherVector3D.getX(), 1.2f);
    QCOMPARE(otherVector3D.getY(), 3.4f);
    QCOMPARE(otherVector3D.getZ(), 5.6f);
}

void TestVector3D::assignmentOperatorToDouble()
{
    Vector3D vector3d;
    vector3d = 1.2f;

    QCOMPARE(vector3d.getX(), 1.2f);
    QCOMPARE(vector3d.getY(), 1.2f);
    QCOMPARE(vector3d.getZ(), 1.2f);
}

void TestVector3D::sumAssignmentOperatorToVector3d()
{
    Vector3D vector3d(1.2f, 3.4f, 5.6f);
    Vector3D otherVector3D(2.2f, 4.4f, 6.6f);;

    vector3d += otherVector3D;

    QCOMPARE(vector3d.getX(), 3.4f);
    QCOMPARE(vector3d.getY(), 7.8f);
    QCOMPARE(vector3d.getZ(), 12.2f);
}

void TestVector3D::sumAssignmentOperatorToDouble()
{
    Vector3D vector3d;
    vector3d  = 1.2f;
    vector3d += 1.2f;

    QCOMPARE(vector3d.getX(), 2.4f);
    QCOMPARE(vector3d.getY(), 2.4f);
    QCOMPARE(vector3d.getZ(), 2.4f);
}

void TestVector3D::subtractionAssignmentOperatorToVector3d()
{
    Vector3D vector3d(1.2f, 3.4f, 5.6f);
    Vector3D otherVector3D(0.5f, 4.4f, 2.0f);

    vector3d -= otherVector3D;

    QCOMPARE(vector3d.getX(), 0.7f);
    QCOMPARE(vector3d.getY(), -1.0f);
    QCOMPARE(vector3d.getZ(), 3.6f);
}

void TestVector3D::subtractionAssignmentOperatorToDouble()
{
    Vector3D vector3d;
    vector3d  = 1.2f;
    vector3d -= 0.6f;

    QCOMPARE(vector3d.getX(), 0.6f);
    QCOMPARE(vector3d.getY(), 0.6f);
    QCOMPARE(vector3d.getZ(), 0.6f);
}

void TestVector3D::dotProductAssingmentToDouble()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    float factor = 3.0f;

    vector3d *= factor;

    QCOMPARE(vector3d.getX(), 3.0f);
    QCOMPARE(vector3d.getY(), 6.0f);
    QCOMPARE(vector3d.getZ(), 9.0f);
}

void TestVector3D::dotProductToDouble()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    float factor = 3.0f;

    Vector3D result = vector3d * factor;

    QCOMPARE(result.getX(), 3.0f);
    QCOMPARE(result.getY(), 6.0f);
    QCOMPARE(result.getZ(), 9.0f);
}

void TestVector3D::crossProductToVector3d()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    Vector3D otherVector3D(4.0f, 5.0f, 6.0f);

    Vector3D result = Vector3D::crossProduct(vector3d, otherVector3D);

    QCOMPARE(result.getX(), -3.0f);
    QCOMPARE(result.getY(), 6.0f);
    QCOMPARE(result.getZ(), -3.0f);
}

void TestVector3D::divisionByDouble()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    float factor = 2.0f;

    Vector3D result = vector3d / factor;

    QCOMPARE(result.getX(), 0.5f);
    QCOMPARE(result.getY(), 1.0f);
    QCOMPARE(result.getZ(), 1.5f);
}

void TestVector3D::sumToVector3d()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    Vector3D otherVector3D(4.0f, 5.0f, 6.0f);

    Vector3D result = vector3d + otherVector3D;

    QCOMPARE(result.getX(), 5.0f);
    QCOMPARE(result.getY(), 7.0f);
    QCOMPARE(result.getZ(), 9.0f);
}

void TestVector3D::sumToDouble()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    float factor = 2.0f;

    Vector3D result = vector3d + factor;

    QCOMPARE(result.getX(), 3.0f);
    QCOMPARE(result.getY(), 4.0f);
    QCOMPARE(result.getZ(), 5.0f);
}

void TestVector3D::subtractionToVector3d()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    Vector3D otherVector3D(6.0f, 5.0f, 4.0f);

    Vector3D result = vector3d - otherVector3D;

    QCOMPARE(result.getX(), -5.0f);
    QCOMPARE(result.getY(), -3.0f);
    QCOMPARE(result.getZ(), -1.0f);
}

void TestVector3D::subtractionToDouble()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    float factor = 2.0f;

    Vector3D result = vector3d - factor;

    QCOMPARE(result.getX(), -1.0f);
    QCOMPARE(result.getY(), 0.0f);
    QCOMPARE(result.getZ(), 1.0f);
}

void TestVector3D::equaltyOperatorEqual()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    Vector3D otherVector3D(1.0f, 2.0f, 3.0f);

    QVERIFY(vector3d == otherVector3D);
}

void TestVector3D::equaltyOperatorDifferent()
{
    Vector3D vector3d(1.1f, 2.0f, 3.0f);
    Vector3D otherVector3D(1.0f, 2.0f, 3.0f);

    QVERIFY(!(vector3d == otherVector3D));
}

void TestVector3D::getModuleSquared()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);

    QCOMPARE(vector3d.getModuleSquared(), 14.0f);
}

void TestVector3D::getModule()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);

    QCOMPARE(vector3d.getModule(), std::sqrt(14.0f));
}

void TestVector3D::getUnitary()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    Vector3D unitary = vector3d.getUnitary();

    QCOMPARE(unitary.getX(), 1.0f / std::sqrt(14.0f));
    QCOMPARE(unitary.getY(), 2.0f / std::sqrt(14.0f));
    QCOMPARE(unitary.getZ(), 3.0f / std::sqrt(14.0f));
}

void TestVector3D::dotDoubleToVector3d()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    float factor = 2.0f;

    Vector3D result = factor * vector3d;

    QCOMPARE(result.getX(), 2.0f);
    QCOMPARE(result.getY(), 4.0f);
    QCOMPARE(result.getZ(), 6.0f);
}

void TestVector3D::sumDoubleToVector3d()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    float factor = 2.0f;

    Vector3D result = factor + vector3d;

    QCOMPARE(result.getX(), 3.0f);
    QCOMPARE(result.getY(), 4.0f);
    QCOMPARE(result.getZ(), 5.0f);
}

void TestVector3D::subtractionDoubleToVector3d()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    float factor = 2.0f;

    Vector3D result = factor - vector3d;

    QCOMPARE(result.getX(), 1.0f);
    QCOMPARE(result.getY(), 0.0f);
    QCOMPARE(result.getZ(), -1.0f);
}

void TestVector3D::signalInversion()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    Vector3D result = -vector3d;

    QCOMPARE(result.getX(), -1.0f);
    QCOMPARE(result.getY(), -2.0f);
    QCOMPARE(result.getZ(), -3.0f);
}

void TestVector3D::implicitAssignmentOperator()
{
    Vector3D vector3d(1.0f, 2.0f, 3.0f);
    Vector3D result = vector3d;

    QCOMPARE(result.getX(), 1.0f);
    QCOMPARE(result.getY(), 2.0f);
    QCOMPARE(result.getZ(), 3.0f);
}
