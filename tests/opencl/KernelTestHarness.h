#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 300
#endif

#ifndef KERNELTESTHARNESS_H
#define KERNELTESTHARNESS_H

#include <CL/opencl.hpp>

#include <memory>
#include <string>

namespace OpenCLWrapper
{
    class Core;
}

class KernelTestHarness
{
    private:
        std::unique_ptr<OpenCLWrapper::Core> core;
        bool ready;

    public:
        KernelTestHarness();
        ~KernelTestHarness();

        bool isReady() const;

        cl_float4 runSmokePassthrough(const cl_float4& input);
};

#endif // KERNELTESTHARNESS_H
