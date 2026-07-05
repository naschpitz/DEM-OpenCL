#include "TestMaterial.h"

#include <QtTest>

TestMaterial::TestMaterial()
{
    this->jsonValue = nlohmann::json::parse(R"({
        "_id": "TestMaterial",
        "material1": "TestMaterialA",
        "material2": "TestMaterialB",
        "distanceThreshold": 4.56,
        "forceType": "inverse_quadratic",
        "dragForceType": "quadratic",
        "coefficients": [1.23, 3.45, 6.78, 9.10, 11.12, 13.14],
        "dragCoefficients": [15.16, 17.18, 19.20, 21.22, 23.24, 25.26]
    })");
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
    // CL structs store cl_float; suffix literals so QCOMPARE compares float vs float.
    QCOMPARE(materialCL.distanceThreshold, 4.56f);
    QCOMPARE(materialCL.forceType, 3);
    QCOMPARE(materialCL.dragForceType, 1);

    QCOMPARE(materialCL.coefficients[0], 1.23f);
    QCOMPARE(materialCL.coefficients[1], 3.45f);
    QCOMPARE(materialCL.coefficients[2], 6.78f);
    QCOMPARE(materialCL.coefficients[3], 9.10f);
    QCOMPARE(materialCL.coefficients[4], 11.12f);
    QCOMPARE(materialCL.coefficients[5], 13.14f);

    QCOMPARE(materialCL.dragCoefficients[0], 15.16f);
    QCOMPARE(materialCL.dragCoefficients[1], 17.18f);
    QCOMPARE(materialCL.dragCoefficients[2], 19.20f);
    QCOMPARE(materialCL.dragCoefficients[3], 21.22f);
    QCOMPARE(materialCL.dragCoefficients[4], 23.24f);
    QCOMPARE(materialCL.dragCoefficients[5], 25.26f);
}
