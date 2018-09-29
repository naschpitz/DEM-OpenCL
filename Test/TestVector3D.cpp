#include "TestVector3D.h"

#include <QTest>
#include <math.h>

TestVector3D::TestVector3D()
{

}

void TestVector3D::emptyConstructor()
{
    Vector3D vector3d;

    QCOMPARE(vector3d.getX(), 0.0);
    QCOMPARE(vector3d.getY(), 0.0);
    QCOMPARE(vector3d.getZ(), 0.0);
}

void TestVector3D::xyzConstructor()
{
    Vector3D vector3d(1.2, 3.4, 5.6);

    QCOMPARE(vector3d.getX(), 1.2);
    QCOMPARE(vector3d.getY(), 3.4);
    QCOMPARE(vector3d.getZ(), 5.6);
}

void TestVector3D::assignmentOperatorToVector3d()
{
    Vector3D vector3d(1.2, 3.4, 5.6);
    Vector3D otherVector3D = vector3d;

    QCOMPARE(otherVector3D.getX(), 1.2);
    QCOMPARE(otherVector3D.getY(), 3.4);
    QCOMPARE(otherVector3D.getZ(), 5.6);
}

void TestVector3D::assignmentOperatorToDouble()
{
    Vector3D vector3d;
    vector3d = 1.2;

    QCOMPARE(vector3d.getX(), 1.2);
    QCOMPARE(vector3d.getY(), 1.2);
    QCOMPARE(vector3d.getZ(), 1.2);
}

void TestVector3D::sumAssignmentOperatorToVector3d()
{
    Vector3D vector3d(1.2, 3.4, 5.6);
    Vector3D otherVector3D(2.2, 4.4, 6.6);;

    vector3d += otherVector3D;

    QCOMPARE(vector3d.getX(), 3.4);
    QCOMPARE(vector3d.getY(), 7.8);
    QCOMPARE(vector3d.getZ(), 12.2);
}

void TestVector3D::sumAssignmentOperatorToDouble()
{
    Vector3D vector3d;
    vector3d  = 1.2;
    vector3d += 1.2;

    QCOMPARE(vector3d.getX(), 2.4);
    QCOMPARE(vector3d.getY(), 2.4);
    QCOMPARE(vector3d.getZ(), 2.4);
}

void TestVector3D::subtractionAssignmentOperatorToVector3d()
{
    Vector3D vector3d(1.2, 3.4, 5.6);
    Vector3D otherVector3D(0.5, 4.4, 2.0);

    vector3d -= otherVector3D;

    QCOMPARE(vector3d.getX(), 0.7);
    QCOMPARE(vector3d.getY(), -1.0);
    QCOMPARE(vector3d.getZ(), 3.6);
}

void TestVector3D::subtractionAssignmentOperatorToDouble()
{
    Vector3D vector3d;
    vector3d  = 1.2;
    vector3d -= 0.6;

    QCOMPARE(vector3d.getX(), 0.6);
    QCOMPARE(vector3d.getY(), 0.6);
    QCOMPARE(vector3d.getZ(), 0.6);
}

void TestVector3D::dotProductAssingmentToDouble()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    double factor = 3.0;

    vector3d *= factor;

    QCOMPARE(vector3d.getX(), 3.0);
    QCOMPARE(vector3d.getY(), 6.0);
    QCOMPARE(vector3d.getZ(), 9.0);
}

void TestVector3D::dotProductToDouble()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    double factor = 3.0;

    Vector3D result = vector3d * factor;

    QCOMPARE(result.getX(), 3.0);
    QCOMPARE(result.getY(), 6.0);
    QCOMPARE(result.getZ(), 9.0);
}

void TestVector3D::crossProductToVector3d()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    Vector3D otherVector3D(4.0, 5.0, 6.0);

    Vector3D result = Vector3D::crossProduct(vector3d, otherVector3D);

    QCOMPARE(result.getX(), -3.0);
    QCOMPARE(result.getY(), 6.0);
    QCOMPARE(result.getZ(), -3.0);
}

void TestVector3D::divisionByDouble()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    double factor = 2.0;

    Vector3D result = vector3d / factor;

    QCOMPARE(result.getX(), 0.5);
    QCOMPARE(result.getY(), 1.0);
    QCOMPARE(result.getZ(), 1.5);
}

void TestVector3D::sumToVector3d()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    Vector3D otherVector3D(4.0, 5.0, 6.0);

    Vector3D result = vector3d + otherVector3D;

    QCOMPARE(result.getX(), 5.0);
    QCOMPARE(result.getY(), 7.0);
    QCOMPARE(result.getZ(), 9.0);
}

void TestVector3D::sumToDouble()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    double factor = 2.0;

    Vector3D result = vector3d + factor;

    QCOMPARE(result.getX(), 3.0);
    QCOMPARE(result.getY(), 4.0);
    QCOMPARE(result.getZ(), 5.0);
}

void TestVector3D::subtractionToVector3d()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    Vector3D otherVector3D(6.0, 5.0, 4.0);

    Vector3D result = vector3d - otherVector3D;

    QCOMPARE(result.getX(), -5.0);
    QCOMPARE(result.getY(), -3.0);
    QCOMPARE(result.getZ(), -1.0);
}

void TestVector3D::subtractionToDouble()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    double factor = 2.0;

    Vector3D result = vector3d - factor;

    QCOMPARE(result.getX(), -1.0);
    QCOMPARE(result.getY(), 0.0);
    QCOMPARE(result.getZ(), 1.0);
}

void TestVector3D::equaltyOperatorEqual()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    Vector3D otherVector3D(1.0, 2.0, 3.0);

    QVERIFY(vector3d == otherVector3D);
}

void TestVector3D::equaltyOperatorDifferent()
{
    Vector3D vector3d(1.1, 2.0, 3.0);
    Vector3D otherVector3D(1.0, 2.0, 3.0);

    QVERIFY(!(vector3d == otherVector3D));
}

void TestVector3D::getModuleSquared()
{
    Vector3D vector3d(1.0, 2.0, 3.0);

    QCOMPARE(vector3d.getModuleSquared(), 14.0);
}

void TestVector3D::getModule()
{
    Vector3D vector3d(1.0, 2.0, 3.0);

    QCOMPARE(vector3d.getModule(), sqrt(14));
}

void TestVector3D::getUnitary()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    Vector3D unitary = vector3d.getUnitary();

    QCOMPARE(unitary.getX(), 1.0 / sqrt(14));
    QCOMPARE(unitary.getY(), 2.0 / sqrt(14));
    QCOMPARE(unitary.getZ(), 3.0 / sqrt(14));
}

void TestVector3D::dotDoubleToVector3d()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    double factor = 2.0;

    Vector3D result = factor * vector3d;

    QCOMPARE(result.getX(), 2.0);
    QCOMPARE(result.getY(), 4.0);
    QCOMPARE(result.getZ(), 6.0);
}

void TestVector3D::sumDoubleToVector3d()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    double factor = 2.0;

    Vector3D result = factor + vector3d;

    QCOMPARE(result.getX(), 3.0);
    QCOMPARE(result.getY(), 4.0);
    QCOMPARE(result.getZ(), 5.0);
}

void TestVector3D::subtractionDoubleToVector3d()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    double factor = 2.0;

    Vector3D result = factor - vector3d;

    QCOMPARE(result.getX(), 1.0);
    QCOMPARE(result.getY(), 0.0);
    QCOMPARE(result.getZ(), -1.0);
}

void TestVector3D::signalInversion()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    Vector3D result = -vector3d;

    QCOMPARE(result.getX(), -1.0);
    QCOMPARE(result.getY(), -2.0);
    QCOMPARE(result.getZ(), -3.0);
}

void TestVector3D::implicitAssignmentOperator()
{
    Vector3D vector3d(1.0, 2.0, 3.0);
    Vector3D result = vector3d;

    QCOMPARE(result.getX(), 1.0);
    QCOMPARE(result.getY(), 2.0);
    QCOMPARE(result.getZ(), 3.0);
}
