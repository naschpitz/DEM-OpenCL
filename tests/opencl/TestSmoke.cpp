#include "TestSmoke.h"

#include <QTest>

TestSmoke::TestSmoke()
{
    this->harness = nullptr;
}

void TestSmoke::initTestCase()
{
    this->harness = new KernelTestHarness();

    if(!this->harness->isReady()) {
        QSKIP("OpenCL device not available or test kernel compilation failed.");
    }
}

void TestSmoke::cleanupTestCase()
{
    delete this->harness;
    this->harness = nullptr;
}

void TestSmoke::passthrough()
{
    cl_float4 input = {1.5f, -2.25f, 3.75f, 0.0f};

    cl_float4 result = this->harness->runSmokePassthrough(input);

    QCOMPARE(result.x, 1.5f);
    QCOMPARE(result.y, -2.25f);
    QCOMPARE(result.z, 3.75f);
    QCOMPARE(result.w, 0.0f);
}
