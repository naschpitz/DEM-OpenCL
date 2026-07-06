#include "../Face.cpp"
#include "../Material.cpp"
#include "../MaterialsManager.cpp"
#include "../NonSolidObject.cpp"
#include "../SolidObject.cpp"
#include "../ObjectsManager.cpp"
#include "../Particle.cpp"
#include "../Scenery.cpp"
#include "../Vector3D.cpp"
#include "../Vertex.cpp"

#include "TestFace.h"
#include "TestMaterial.h"
#include "TestMaterialsManager.h"
#include "TestNonSolidObject.h"
#include "TestSolidObject.h"
#include "TestObjectsManager.h"
#include "TestParticle.h"
#include "TestScenery.h"
#include "TestVector3D.h"
#include "TestVertex.h"

#include <QObject>
#include <QTest>
#include <QCoreApplication>
#include <clocale>
#include <iostream>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  TestFace testFace;
  TestMaterial testMaterial;
  TestMaterialsManager testMaterialsManager;
  TestNonSolidObject testNonSolidObject;
  TestObjectsManager testObjectsManager;
  TestParticle testParticle;
  TestScenery testScenery;
  TestSolidObject testSolidObject;
  TestVector3D testVector3D;
  TestVertex testVertex;

  QTest::qExec(&testFace);
  QTest::qExec(&testMaterial);
  QTest::qExec(&testMaterialsManager);
  QTest::qExec(&testNonSolidObject);
  QTest::qExec(&testObjectsManager);
  QTest::qExec(&testParticle);
  QTest::qExec(&testScenery);
  QTest::qExec(&testSolidObject);
  QTest::qExec(&testVector3D);
  QTest::qExec(&testVertex);

  std::lconv* lc = std::localeconv();
  std::cout << "Decimal Separator: " << lc->decimal_point << "\n";

  return 0;
}
