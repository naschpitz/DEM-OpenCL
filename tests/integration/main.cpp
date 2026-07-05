#include "TestSimulationConfig.h"

#include <QObject>
#include <QTest>
#include <QCoreApplication>
#include <iostream>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    TestSimulationConfig testSimulationConfig;
    int failed = 0;

    failed += QTest::qExec(&testSimulationConfig);

    std::cout << (failed == 0 ? "INTEGRATION: All tests passed" : "INTEGRATION: FAILURES")
              << std::endl;

    return failed;
}
