#include "TestSolidObject.h"
#include "TestHelpers.h"

#include <QTest>
#include <math.h>

TestSolidObject::TestSolidObject()
{
    // Hand-authored fixture: a clean 60 x 2 x 10 box (origin at 0,0,0) plus the
    // same scalar fields the old SolidObject.json carried. The "stl" field is
    // generated procedurally so the SolidObject(nlohmann::json) -> loadStl() ->
    // tetgen pipeline is still exercised end-to-end, without a hand-written
    // ASCII-STL blob embedded in a JSON file. initialize() -> setPosition()
    // displaces every vertex by this->position, so getBox() bounds are
    // boxMin + position / boxMax + position.
    this->solidObjectJsonValue = nlohmann::json{
        {"_id", "TestSolidObject"},
        {"material", "6pwfsYxGumpgZzAfP"},
        {"mass", 7.07},
        {"fixed", false},
        {"position", nlohmann::json::array_t{1.01, 2.02, 3.03}},
        {"velocity", nlohmann::json::array_t{4.04, 5.05, 6.06}},
        {"stl", makeBoxSTL(0.0, 0.0, 0.0, 60.0, 2.0, 10.0)}
    };
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
    // ([1.01, 2.02, 3.03]). The box mesh is a clean (0,0,0)->(60,2,10), so after
    // displacement the bounds are (1.01,2.02,3.03)->(61.01,4.02,13.03). Vector3D
    // stores float; QCOMPARE(Vector3D) uses exact ==, so compare each component
    // with a small absolute tolerance instead.
    QVERIFY(qAbs(min.getX() - 1.01f) < 1e-3f);
    QVERIFY(qAbs(min.getY() - 2.02f) < 1e-3f);
    QVERIFY(qAbs(min.getZ() - 3.03f) < 1e-3f);

    QVERIFY(qAbs(max.getX() - 61.01f) < 1e-3f);
    QVERIFY(qAbs(max.getY() - 4.02f) < 1e-3f);
    QVERIFY(qAbs(max.getZ() - 13.03f) < 1e-3f);
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
