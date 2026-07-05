#include "TestSolidObject.h"

#include <QTest>
#include <fstream>
#include <math.h>

TestSolidObject::TestSolidObject()
{
    std::ifstream file("../SolidObject.json");
    this->solidObjectJsonValue = nlohmann::json::parse(file);
}

void TestSolidObject::constructor()
{
    SolidObject solidObject(this->solidObjectJsonValue);

    double mass = solidObject.getMass();
    double expectedMass = 7.07;

    QVERIFY(mass <= expectedMass + 0.0001);
    QVERIFY(mass >= expectedMass - 0.0001);
}

void TestSolidObject::getMaterialId()
{
    SolidObject solidObject(this->solidObjectJsonValue);

    const QString expectedMaterial = "6pwfsYxGumpgZzAfP";

    QVERIFY(solidObject.getMaterial() == expectedMaterial);
}

void TestSolidObject::getBox()
{
    SolidObject solidObject(this->solidObjectJsonValue);
    solidObject.initialize();

    Vector3D min, max;
    solidObject.getBox(min, max);

    // initialize() -> setPosition() displaces every vertex by this->position
    // ([1.01, 2.02, 3.03]). The tetgen mesh is a clean (0,-1,-5)->(60,1,5) box,
    // so after displacement the bounds are as below. Vector3D stores float;
    // QCOMPARE(Vector3D) uses exact ==, so compare each component with a small
    // absolute tolerance instead.
    QVERIFY(qAbs(min.getX() - 1.01f) < 1e-3f);
    QVERIFY(qAbs(min.getY() - 1.02f) < 1e-3f);
    QVERIFY(qAbs(min.getZ() - (-1.97f)) < 1e-3f);

    QVERIFY(qAbs(max.getX() - 61.01f) < 1e-3f);
    QVERIFY(qAbs(max.getY() - 3.02f) < 1e-3f);
    QVERIFY(qAbs(max.getZ() - 8.03f) < 1e-3f);
}

void TestSolidObject::getCurrentMomentum()
{
    SolidObject solidObject(this->solidObjectJsonValue);
    solidObject.initialize();

    // initialize() -> setMass() distributes this->mass across faces by area, so
    // the summed face mass equals the object mass.
    double expectedMass = 7.07;

    Vector3D currentMomentum = solidObject.getCurrentMomentum();
    Vector3D expectedMomentum(expectedMass * 4.04, expectedMass * 5.05, expectedMass * 6.06);

    QVERIFY(currentMomentum.getX() <= expectedMomentum.getX() + 0.0001);
    QVERIFY(currentMomentum.getX() >= expectedMomentum.getX() - 0.0001);
    QVERIFY(currentMomentum.getY() <= expectedMomentum.getY() + 0.0001);
    QVERIFY(currentMomentum.getY() >= expectedMomentum.getY() - 0.0001);
    QVERIFY(currentMomentum.getZ() <= expectedMomentum.getZ() + 0.0001);
    QVERIFY(currentMomentum.getZ() >= expectedMomentum.getZ() - 0.0001);
}

void TestSolidObject::getCurrentKineticEnergyTotal()
{
    SolidObject solidObject(this->solidObjectJsonValue);
    solidObject.initialize();

    double expectedMass = 7.07;

    Vector3D expectedVelocity(4.04, 5.05, 6.06);

    double currentKineticEnergyTotal = solidObject.getCurrentKineticEnergyTotal();
    double expectedKineticEnergyTotal = expectedMass * expectedVelocity.getModuleSquared() / 2.0;

    QVERIFY(currentKineticEnergyTotal <= expectedKineticEnergyTotal + 0.0001);
    QVERIFY(currentKineticEnergyTotal >= expectedKineticEnergyTotal - 0.0001);
}
