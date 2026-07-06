#include "TestMaterialsManager.h"

#include <QtTest>

TestMaterialsManager::TestMaterialsManager()
{
  this->jsonArray = nlohmann::json::parse(R"({
        "root": [
            {
                "_id": "TestMaterial0",
                "distanceThreshold": 4.560,
                "forceType": "inverse_quadratic",
                "dragForceType": "quadratic",
                "coefficients": [1.230, 3.450, 6.780, 9.100, 11.120, 13.140],
                "dragCoefficients": [15.160, 17.180, 19.200, 21.220, 23.240, 25.260]
            },
            {
                "_id": "TestMaterial1",
                "distanceThreshold": 4.561,
                "forceType": "inverse_quadratic",
                "dragForceType": "quadratic",
                "coefficients": [1.231, 3.451, 6.781, 9.101, 11.121, 13.141],
                "dragCoefficients": [15.161, 17.181, 19.201, 21.221, 23.241, 25.261]
            },
            {
                "_id": "TestMaterial2",
                "material1": "TestMaterial0",
                "material2": "TestMaterial1",
                "distanceThreshold": 4.562,
                "forceType": "inverse_quadratic",
                "dragForceType": "quadratic",
                "coefficients": [1.232, 3.452, 6.782, 9.102, 11.122, 13.142],
                "dragCoefficients": [15.162, 17.182, 19.202, 21.222, 23.242, 25.262]
            }
        ]
    })")["root"];
}

void TestMaterialsManager::constructor()
{
  MaterialsManager materialsManager(this->jsonArray);

  const QVector<Material>& materials = materialsManager.getMaterials();

  QVERIFY(materials.count() == 3);

  // Material 0
  QCOMPARE(materials[0].getId(), "TestMaterial0");
  QCOMPARE(materials[0].getDistanceThreshold(), 4.560);
  QCOMPARE(materials[0].getForceType(), "inverse_quadratic");
  QCOMPARE(materials[0].getDragForceType(), "quadratic");

  QVERIFY(materials[0].getCoefficients().size() == 6);
  QCOMPARE(materials[0].getCoefficients().at(0), 1.230);
  QCOMPARE(materials[0].getCoefficients().at(1), 3.450);
  QCOMPARE(materials[0].getCoefficients().at(2), 6.780);
  QCOMPARE(materials[0].getCoefficients().at(3), 9.100);
  QCOMPARE(materials[0].getCoefficients().at(4), 11.120);
  QCOMPARE(materials[0].getCoefficients().at(5), 13.140);

  QVERIFY(materials[0].getDragCoefficients().size() == 6);
  QCOMPARE(materials[0].getDragCoefficients().at(0), 15.160);
  QCOMPARE(materials[0].getDragCoefficients().at(1), 17.180);
  QCOMPARE(materials[0].getDragCoefficients().at(2), 19.200);
  QCOMPARE(materials[0].getDragCoefficients().at(3), 21.220);
  QCOMPARE(materials[0].getDragCoefficients().at(4), 23.240);
  QCOMPARE(materials[0].getDragCoefficients().at(5), 25.260);

  // Material 1
  QCOMPARE(materials[1].getId(), "TestMaterial1");
  QCOMPARE(materials[1].getDistanceThreshold(), 4.561);
  QCOMPARE(materials[1].getForceType(), "inverse_quadratic");
  QCOMPARE(materials[1].getDragForceType(), "quadratic");

  QVERIFY(materials[1].getCoefficients().size() == 6);
  QCOMPARE(materials[1].getCoefficients().at(0), 1.231);
  QCOMPARE(materials[1].getCoefficients().at(1), 3.451);
  QCOMPARE(materials[1].getCoefficients().at(2), 6.781);
  QCOMPARE(materials[1].getCoefficients().at(3), 9.101);
  QCOMPARE(materials[1].getCoefficients().at(4), 11.121);
  QCOMPARE(materials[1].getCoefficients().at(5), 13.141);

  QVERIFY(materials[1].getDragCoefficients().size() == 6);
  QCOMPARE(materials[1].getDragCoefficients().at(0), 15.161);
  QCOMPARE(materials[1].getDragCoefficients().at(1), 17.181);
  QCOMPARE(materials[1].getDragCoefficients().at(2), 19.201);
  QCOMPARE(materials[1].getDragCoefficients().at(3), 21.221);
  QCOMPARE(materials[1].getDragCoefficients().at(4), 23.241);
  QCOMPARE(materials[1].getDragCoefficients().at(5), 25.261);

  // Material 2
  QCOMPARE(materials[2].getId(), "TestMaterial2");
  QCOMPARE(materials[2].getMaterial1(), "TestMaterial0");
  QCOMPARE(materials[2].getMaterial2(), "TestMaterial1");
  QCOMPARE(materials[2].getDistanceThreshold(), 4.562);
  QCOMPARE(materials[2].getForceType(), "inverse_quadratic");
  QCOMPARE(materials[2].getDragForceType(), "quadratic");

  QVERIFY(materials[2].getCoefficients().size() == 6);
  QCOMPARE(materials[2].getCoefficients().at(0), 1.232);
  QCOMPARE(materials[2].getCoefficients().at(1), 3.452);
  QCOMPARE(materials[2].getCoefficients().at(2), 6.782);
  QCOMPARE(materials[2].getCoefficients().at(3), 9.102);
  QCOMPARE(materials[2].getCoefficients().at(4), 11.122);
  QCOMPARE(materials[2].getCoefficients().at(5), 13.142);

  QVERIFY(materials[2].getDragCoefficients().size() == 6);
  QCOMPARE(materials[2].getDragCoefficients().at(0), 15.162);
  QCOMPARE(materials[2].getDragCoefficients().at(1), 17.182);
  QCOMPARE(materials[2].getDragCoefficients().at(2), 19.202);
  QCOMPARE(materials[2].getDragCoefficients().at(3), 21.222);
  QCOMPARE(materials[2].getDragCoefficients().at(4), 23.242);
  QCOMPARE(materials[2].getDragCoefficients().at(5), 25.262);
}

void TestMaterialsManager::getCL()
{
  MaterialsManager materialsManager(this->jsonArray);

  MaterialsManagerCL materialsManagerCL = materialsManager.getCL();

  QVERIFY(materialsManagerCL.size == 3);

  // CL structs store cl_float; suffix literals so QCOMPARE compares float vs float.
  // Material 0
  MaterialCL& materialCL0 = materialsManagerCL.materials[0];
  QCOMPARE(materialCL0.materialIndex1, -1);
  QCOMPARE(materialCL0.materialIndex2, -1);
  QCOMPARE(materialCL0.distanceThreshold, 4.560f);
  QCOMPARE(materialCL0.forceType, 3);
  QCOMPARE(materialCL0.dragForceType, 1);

  QCOMPARE(materialCL0.coefficients[0], 1.230f);
  QCOMPARE(materialCL0.coefficients[1], 3.450f);
  QCOMPARE(materialCL0.coefficients[2], 6.780f);
  QCOMPARE(materialCL0.coefficients[3], 9.100f);
  QCOMPARE(materialCL0.coefficients[4], 11.120f);
  QCOMPARE(materialCL0.coefficients[5], 13.140f);

  QCOMPARE(materialCL0.dragCoefficients[0], 15.160f);
  QCOMPARE(materialCL0.dragCoefficients[1], 17.180f);
  QCOMPARE(materialCL0.dragCoefficients[2], 19.200f);
  QCOMPARE(materialCL0.dragCoefficients[3], 21.220f);
  QCOMPARE(materialCL0.dragCoefficients[4], 23.240f);
  QCOMPARE(materialCL0.dragCoefficients[5], 25.260f);

  // Material 1
  MaterialCL& materialCL1 = materialsManagerCL.materials[1];
  QCOMPARE(materialCL1.materialIndex1, -1);
  QCOMPARE(materialCL1.materialIndex2, -1);
  QCOMPARE(materialCL1.distanceThreshold, 4.561f);
  QCOMPARE(materialCL1.forceType, 3);
  QCOMPARE(materialCL1.dragForceType, 1);

  QCOMPARE(materialCL1.coefficients[0], 1.231f);
  QCOMPARE(materialCL1.coefficients[1], 3.451f);
  QCOMPARE(materialCL1.coefficients[2], 6.781f);
  QCOMPARE(materialCL1.coefficients[3], 9.101f);
  QCOMPARE(materialCL1.coefficients[4], 11.121f);
  QCOMPARE(materialCL1.coefficients[5], 13.141f);

  QCOMPARE(materialCL1.dragCoefficients[0], 15.161f);
  QCOMPARE(materialCL1.dragCoefficients[1], 17.181f);
  QCOMPARE(materialCL1.dragCoefficients[2], 19.201f);
  QCOMPARE(materialCL1.dragCoefficients[3], 21.221f);
  QCOMPARE(materialCL1.dragCoefficients[4], 23.241f);
  QCOMPARE(materialCL1.dragCoefficients[5], 25.261f);

  // Material 2
  MaterialCL& materialCL2 = materialsManagerCL.materials[2];
  QCOMPARE(materialCL2.materialIndex1, 0);
  QCOMPARE(materialCL2.materialIndex2, 1);
  QCOMPARE(materialCL2.distanceThreshold, 4.562f);
  QCOMPARE(materialCL2.forceType, 3);
  QCOMPARE(materialCL2.dragForceType, 1);

  QCOMPARE(materialCL2.coefficients[0], 1.232f);
  QCOMPARE(materialCL2.coefficients[1], 3.452f);
  QCOMPARE(materialCL2.coefficients[2], 6.782f);
  QCOMPARE(materialCL2.coefficients[3], 9.102f);
  QCOMPARE(materialCL2.coefficients[4], 11.122f);
  QCOMPARE(materialCL2.coefficients[5], 13.142f);

  QCOMPARE(materialCL2.dragCoefficients[0], 15.162f);
  QCOMPARE(materialCL2.dragCoefficients[1], 17.182f);
  QCOMPARE(materialCL2.dragCoefficients[2], 19.202f);
  QCOMPARE(materialCL2.dragCoefficients[3], 21.222f);
  QCOMPARE(materialCL2.dragCoefficients[4], 23.242f);
  QCOMPARE(materialCL2.dragCoefficients[5], 25.262f);
}
