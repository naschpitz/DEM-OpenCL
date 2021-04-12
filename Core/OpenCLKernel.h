#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 210
#endif

#ifndef OPENCLKERNEL_H
#define OPENCLKERNEL_H

#include <CL/cl2.hpp>

namespace OpenCL
{
    struct Kernel
    {
        std::string name;
        cl_uint nElements;
        cl::Kernel kernel;
        uint argsCount = 0;
    };
}

#endif // OPENCLKERNEL_H
