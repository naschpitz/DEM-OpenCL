#include <QCoreApplication>
#include <QTest>

#include <OCLW_Core.hpp>

#include <clocale>

#include "TestSmoke.h"
#include "TestVector.h"
#include "TestEdge.h"
#include "TestParticle.h"
#include "TestFace.h"
#include "TestObject.h"
#include "TestNonConvexObject.h"
#include "TestObjectToObject.h"
#include "TestMaterial.h"
#include "TestParticleWorker.h"

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  std::setlocale(LC_NUMERIC, "C");

  OpenCLWrapper::Core::initialize();

  TestSmoke testSmoke;
  TestVector testVector;
  TestEdge testEdge;
  TestParticle testParticle;
  TestFace testFace;
  TestObject testObject;
  TestNonConvexObject testNonConvexObject;
  TestObjectToObject testObjectToObject;
  TestMaterial testMaterial;
  TestParticleWorker testParticleWorker;

  int failures = 0;
  failures += QTest::qExec(&testSmoke);
  failures += QTest::qExec(&testVector);
  failures += QTest::qExec(&testEdge);
  failures += QTest::qExec(&testParticle);
  failures += QTest::qExec(&testFace);
  failures += QTest::qExec(&testObject);
  failures += QTest::qExec(&testNonConvexObject);
  failures += QTest::qExec(&testObjectToObject);
  failures += QTest::qExec(&testMaterial);
  failures += QTest::qExec(&testParticleWorker);

  return failures;
}
