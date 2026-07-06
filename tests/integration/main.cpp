#include "TestSimulationConfig.h"
#include "TestSimulationPhysics.h"
#include "TestSimulationContacts.h"
#include "TestSimulationCollisions.h"
#include "TestRequestMapper.h"

#include <QObject>
#include <QTest>
#include <QCoreApplication>
#include <iostream>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  int failed = 0;

  TestSimulationConfig testSimulationConfig;
  TestSimulationPhysics testSimulationPhysics;
  TestSimulationContacts testSimulationContacts;
  TestSimulationCollisions testSimulationCollisions;
  TestRequestMapper testRequestMapper;

  failed += QTest::qExec(&testSimulationConfig);
  failed += QTest::qExec(&testSimulationPhysics);
  failed += QTest::qExec(&testSimulationContacts);
  failed += QTest::qExec(&testSimulationCollisions);
  failed += QTest::qExec(&testRequestMapper);

  std::cout << (failed == 0 ? "INTEGRATION: All tests passed" : "INTEGRATION: FAILURES") << std::endl;

  return failed;
}
