#include "KernelTestHarness.h"

#include <OCLW_Core.hpp>

namespace
{
    const std::string KERNEL_SOURCE_PATH = "../opencl/Tests.cpp.cl";
}

KernelTestHarness::KernelTestHarness()
{
    this->ready = false;

    try {
        this->core = std::make_unique<OpenCLWrapper::Core>(false);
        this->core->addSourceFile(KERNEL_SOURCE_PATH);
        this->ready = true;
    }

    catch (...) {
        this->core.reset();
        this->ready = false;
    }
}

KernelTestHarness::~KernelTestHarness() = default;

bool KernelTestHarness::isReady() const
{
    return this->ready;
}

cl_float4 KernelTestHarness::runSmokePassthrough(const cl_float4& input)
{
    std::vector<cl_float4> inputVec = { input };
    std::vector<cl_float4> outputVec = { cl_float4{0.0f, 0.0f, 0.0f, 0.0f} };

    this->core->writeBuffer<cl_float4>("smoke_input", inputVec, 0);
    this->core->writeBuffer<cl_float4>("smoke_output", outputVec, 0);

    this->core->addKernel("test_smoke_passthrough", 1);
    this->core->addArgument<cl_float4>("test_smoke_passthrough", "smoke_input");
    this->core->addArgument<cl_float4>("test_smoke_passthrough", "smoke_output");

    this->core->run();
    this->core->clearKernels();

    this->core->readBuffer<cl_float4>("smoke_output", outputVec, 0);

    return outputVec[0];
}
