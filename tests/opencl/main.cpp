#include <QCoreApplication>
#include <QTest>

#include <OCLW_Core.hpp>

#include <clocale>

#include "TestSmoke.h"

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    std::setlocale(LC_NUMERIC, "C");

    OpenCLWrapper::Core::initialize();

    TestSmoke testSmoke;

    int failures = 0;
    failures += QTest::qExec(&testSmoke);

    return failures;
}
