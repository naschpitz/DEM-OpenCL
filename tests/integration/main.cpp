#include "TestSimulationConfig.h"
#include "TestSimulationPhysics.h"

#include <QObject>
#include <QTest>
#include <QCoreApplication>
#include <iostream>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    int failed = 0;

    TestSimulationConfig   testSimulationConfig;
    TestSimulationPhysics  testSimulationPhysics;

    failed += QTest::qExec(&testSimulationConfig);
    failed += QTest::qExec(&testSimulationPhysics);

    std::cout << (failed == 0 ? "INTEGRATION: All tests passed" : "INTEGRATION: FAILURES")
              << std::endl;

    return failed;
}
