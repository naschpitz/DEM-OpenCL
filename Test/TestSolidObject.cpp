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

    Vector3D expectedPosition(30.007183295555333, -0.00075968578842276378, -0.00029202407892601811);
    Vector3D expectedVelocity(4.04, 5.05, 6.06);

    Vector3D currentPosition = solidObject.getCurrentPosition();

    QCOMPARE(solidObject.getCurrentPosition(), expectedPosition);
    QCOMPARE(solidObject.getCurrentVelocity(), expectedVelocity);

    double expectedMass = 7.07;

    QVERIFY(solidObject.getMass() <= expectedMass + 0.0001);
    QVERIFY(solidObject.getMass() >= expectedMass - 0.0001);
}

void TestSolidObject::getMaterialId()
{
    SolidObject solidObject(this->solidObjectJsonValue);

    const QString expectedMaterialId = "6pwfsYxGumpgZzAfP";

    QVERIFY(solidObject.getMaterialId() == expectedMaterialId);
}

void TestSolidObject::getBox()
{
    SolidObject solidObject(this->solidObjectJsonValue);

    Vector3D min, max;
    solidObject.getBox(min, max);

    Vector3D expectedMin, expectedMax;

    expectedMin.setX(-0.31425321690986274);
    expectedMin.setY(-1.3142532169098629);
    expectedMin.setZ(-5.3142532169098624);

    expectedMax.setX(60.314253216909862);
    expectedMax.setY(1.3142532169098629);
    expectedMax.setZ(5.3142532169098624);

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
