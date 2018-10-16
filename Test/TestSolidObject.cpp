#include "TestSolidObject.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QTest>
#include <math.h>

TestSolidObject::TestSolidObject()
{
    QFile solidObjectFile("../SolidObject.json");
    solidObjectFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QString solidObjectFileString = solidObjectFile.readAll();
    solidObjectFile.close();

    QJsonDocument solidObjectJsonDocument = QJsonDocument::fromJson(solidObjectFileString.toUtf8());
    this->solidObjectJsonValue = solidObjectJsonDocument.object();
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

    Vector3D min, max;
    solidObject.getBox(min, max);

    Vector3D expectedMin, expectedMax;

    expectedMin.setX( 1.0000000000000000);
    expectedMin.setY( 1.0100000000000000);
    expectedMin.setZ(-1.9800000000000000);

    expectedMax.setX(61.019999999999999);
    expectedMax.setY(3.0300000000000000);
    expectedMax.setZ(8.0399999999999999);

    QCOMPARE(min, expectedMin);
    QCOMPARE(max, expectedMax);
}

void TestSolidObject::getCurrentMomentum()
{
    SolidObject solidObject(this->solidObjectJsonValue);

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

    double expectedMass = 7.07;

    Vector3D expectedVelocity(4.04, 5.05, 6.06);

    double currentKineticEnergyTotal = solidObject.getCurrentKineticEnergyTotal();
    double expectedKineticEnergyTotal = expectedMass * expectedVelocity.getModuleSquared() / 2.0;

    QVERIFY(currentKineticEnergyTotal <= expectedKineticEnergyTotal + 0.0001);
    QVERIFY(currentKineticEnergyTotal >= expectedKineticEnergyTotal - 0.0001);
}
