#include "TestMaterial.h"

#include <QString>
#include <QString>
#include <QtTest>

TestMaterial::TestMaterial()
{
    QFile file("../Material.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QString fileString = file.readAll();
    file.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(fileString.toUtf8());
    this->jsonValue = jsonDocument.object();
}

void TestMaterial::constructor()
{
    Material material(this->jsonValue);

    QCOMPARE(material.getId(), "TestMaterial");
    QCOMPARE(material.getMaterial1(), "TestMaterialA");
    QCOMPARE(material.getMaterial2(), "TestMaterialB");
    QCOMPARE(material.getDistanceThreshold(), 4.56);
    QCOMPARE(material.getForceType(), "inverse_quadratic");
    QCOMPARE(material.getDragForceType(), "quadratic");

    QVERIFY(material.getCoefficients().size() == 6);
    QCOMPARE(material.getCoefficients().at(0), 1.23);
    QCOMPARE(material.getCoefficients().at(1), 3.45);
    QCOMPARE(material.getCoefficients().at(2), 6.78);
    QCOMPARE(material.getCoefficients().at(3), 9.10);
    QCOMPARE(material.getCoefficients().at(4), 11.12);
    QCOMPARE(material.getCoefficients().at(5), 13.14);

    QVERIFY(material.getDragCoefficients().size() == 6);
    QCOMPARE(material.getDragCoefficients().at(0), 15.16);
    QCOMPARE(material.getDragCoefficients().at(1), 17.18);
    QCOMPARE(material.getDragCoefficients().at(2), 19.20);
    QCOMPARE(material.getDragCoefficients().at(3), 21.22);
    QCOMPARE(material.getDragCoefficients().at(4), 23.24);
    QCOMPARE(material.getDragCoefficients().at(5), 25.26);
}

void TestMaterial::getCL()
{
    QMap<QString, int> namesMap;
    namesMap["TestMaterial"]  = 0;
    namesMap["TestMaterialA"] = 1;
    namesMap["TestMaterialB"] = 2;

    Material material(this->jsonValue);

    MaterialCL materialCL = material.getCL(namesMap);

    QCOMPARE(materialCL.materialIndex1, 1);
    QCOMPARE(materialCL.materialIndex2, 2);
    QCOMPARE(materialCL.distanceThreshold, 4.56);
    QCOMPARE(materialCL.forceType, 2);
    QCOMPARE(materialCL.dragForceType, 1);

    QCOMPARE(materialCL.coefficients[0], 1.23);
    QCOMPARE(materialCL.coefficients[1], 3.45);
    QCOMPARE(materialCL.coefficients[2], 6.78);
    QCOMPARE(materialCL.coefficients[3], 9.10);
    QCOMPARE(materialCL.coefficients[4], 11.12);
    QCOMPARE(materialCL.coefficients[5], 13.14);

    QCOMPARE(materialCL.dragCoefficients[0], 15.16);
    QCOMPARE(materialCL.dragCoefficients[1], 17.18);
    QCOMPARE(materialCL.dragCoefficients[2], 19.20);
    QCOMPARE(materialCL.dragCoefficients[3], 21.22);
    QCOMPARE(materialCL.dragCoefficients[4], 23.24);
    QCOMPARE(materialCL.dragCoefficients[5], 25.26);
}
