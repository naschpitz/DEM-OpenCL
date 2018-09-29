#include "TestNonSolidObject.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QTest>
#include <math.h>

TestNonSolidObject::TestNonSolidObject()
{
    QFile nonSolidObjectWireframeFile("../NonSolidObjectWireframe.json");
    nonSolidObjectWireframeFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QString nonSolidObjectWireframeFileString = nonSolidObjectWireframeFile.readAll();
    nonSolidObjectWireframeFile.close();

    QJsonDocument nonSolidObjectWireframeJsonDocument = QJsonDocument::fromJson(nonSolidObjectWireframeFileString.toUtf8());
    this->nonSolidObjectWireframeJsonValue = nonSolidObjectWireframeJsonDocument.object();

    //-----------------------------------------------------------------//

    QFile nonSolidObjectInstanceFile("../NonSolidObjectInstance.json");
    nonSolidObjectInstanceFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QString nonSolidObjectInstanceFileString = nonSolidObjectInstanceFile.readAll();
    nonSolidObjectInstanceFile.close();

    QJsonDocument nonSolidObjectInstanceJsonDocument = QJsonDocument::fromJson(nonSolidObjectInstanceFileString.toUtf8());
    this->nonSolidObjectInstanceJsonValue = nonSolidObjectInstanceJsonDocument.object();
}

void TestNonSolidObject::constructorWireframe()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);

    Vector3D expectedPosition(0, 0, 0);
    Vector3D expectedVelocity(4.04, 5.05, 6.06);

    QCOMPARE(nonSolidObject.getCurrentPosition(), expectedPosition);
    QCOMPARE(nonSolidObject.getCurrentVelocity(), expectedVelocity);

    double expectedVolume = 523.5987756;
    double density        = 7.07;
    double expectedMass   = expectedVolume * density;

    QVERIFY(nonSolidObject.getCurrentMass() <= expectedMass + 0.0001);
    QVERIFY(nonSolidObject.getCurrentMass() >= expectedMass - 0.0001);
}

void TestNonSolidObject::constructorInstance()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectInstanceJsonValue);

    Vector3D expectedPosition(0, 1, 2);
    Vector3D expectedVelocity(9, 10, 11);

    QCOMPARE(nonSolidObject.getCurrentPosition(), expectedPosition);
    QCOMPARE(nonSolidObject.getCurrentVelocity(), expectedVelocity);

    double expectedVolume = 0.5236;
    double density        = 7.07;
    double expectedMass   = expectedVolume * density;

    QVERIFY(nonSolidObject.getCurrentMass() <= expectedMass + 0.0001);
    QVERIFY(nonSolidObject.getCurrentMass() >= expectedMass - 0.0001);
}

void TestNonSolidObject::getMaterialId()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);

    const QString expectedMaterialId = "6pwfsYxGumpgZzAfP";

    QVERIFY(nonSolidObject.getMaterialId() == expectedMaterialId);
}

void TestNonSolidObject::getBox()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);

    Vector3D min, max;
    nonSolidObject.getBox(min, max);

    Vector3D expectedMin, expectedMax;

    expectedMin.setX(-5);
    expectedMin.setY(-5);
    expectedMin.setZ(-5);

    expectedMax.setX(5);
    expectedMax.setY(5);
    expectedMax.setZ(5);

    QCOMPARE(min, expectedMin);
    QCOMPARE(max, expectedMax);
}

void TestNonSolidObject::getCurrentMomentum()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);

    double expectedVolume = 523.5987756;
    double density        = 7.07;
    double expectedMass   = expectedVolume * density;

    Vector3D currentMomentum = nonSolidObject.getCurrentMomentum();
    Vector3D expectedMomentum(expectedMass * 4.04, expectedMass * 5.05, expectedMass * 6.06);

    QVERIFY(currentMomentum.getX() <= expectedMomentum.getX() + 0.0001);
    QVERIFY(currentMomentum.getX() >= expectedMomentum.getX() - 0.0001);
    QVERIFY(currentMomentum.getY() <= expectedMomentum.getY() + 0.0001);
    QVERIFY(currentMomentum.getY() >= expectedMomentum.getY() - 0.0001);
    QVERIFY(currentMomentum.getZ() <= expectedMomentum.getZ() + 0.0001);
    QVERIFY(currentMomentum.getZ() >= expectedMomentum.getZ() - 0.0001);
}

void TestNonSolidObject::getCurrentKineticEnergyTotal()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);

    double expectedVolume = 523.5987756;
    double density        = 7.07;
    double expectedMass   = expectedVolume * density;

    Vector3D expectedVelocity(4.04, 5.05, 6.06);

    double currentKineticEnergyTotal = nonSolidObject.getCurrentKineticEnergyTotal();
    double expectedKineticEnergyTotal = expectedMass * expectedVelocity.getModuleSquared() / 2.0;

    QVERIFY(currentKineticEnergyTotal <= expectedKineticEnergyTotal + 0.0001);
    QVERIFY(currentKineticEnergyTotal >= expectedKineticEnergyTotal - 0.0001);
}

void TestNonSolidObject::getCurrentKineticEnergyExternal()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);

    double expectedVolume = 523.5987756;
    double density        = 7.07;
    double expectedMass   = expectedVolume * density;

    Vector3D expectedVelocity(4.04, 5.05, 6.06);

    double currentKineticEnergyExternal = nonSolidObject.getCurrentKineticEnergyExternal();
    double expectedKineticEnergyExtrnal = expectedMass * expectedVelocity.getModuleSquared() / 2.0;

    QVERIFY(currentKineticEnergyExternal <= expectedKineticEnergyExtrnal + 0.0001);
    QVERIFY(currentKineticEnergyExternal >= expectedKineticEnergyExtrnal - 0.0001);
}

void TestNonSolidObject::getCurrentKineticEnergyInternal()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);

    double currentKineticEnergyInternal = nonSolidObject.getCurrentKineticEnergyInternal();
    double expectedKineticEnergyInternal = 0.0;

    QVERIFY(currentKineticEnergyInternal <= expectedKineticEnergyInternal + 0.0001);
    QVERIFY(currentKineticEnergyInternal >= expectedKineticEnergyInternal - 0.0001);
}
