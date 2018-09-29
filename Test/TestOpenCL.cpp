#include "TestOpenCL.h"

#include <QString>
#include <QtTest>

TestOpenCL::TestOpenCL()
{
    OpenCL::Core::showDevices();
}

void TestOpenCL::test()
{
    OpenCL::Core core;

    core.addSourceFile("../Test.cl");

    std::vector<int> A = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> B = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};
    std::vector<int> C = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    core.writeBuffer<int>(A);
    core.writeBuffer<int>(B);
    core.writeBuffer<int>(C);

    core.addKernel("simple_add", 10);
    core.addArgument<int>("simple_add", A);
    core.addArgument<int>("simple_add", B);
    core.addArgument<int>("simple_add", C);

    core.run();

    core.readBuffer(C);

    QCOMPARE(C[0], 0);
    QCOMPARE(C[1], 2);
    QCOMPARE(C[2], 4);
    QCOMPARE(C[3], 3);
    QCOMPARE(C[4], 5);
    QCOMPARE(C[5], 7);
    QCOMPARE(C[6], 6);
    QCOMPARE(C[7], 8);
    QCOMPARE(C[8], 10);
    QCOMPARE(C[9], 9);
}

void TestOpenCL::simulation()
{

}
