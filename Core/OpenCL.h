#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#ifndef OPENCL_H
#define OPENCL_H

#include <CL/cl2.hpp>
#include <iostream>
#include <unistd.h>

#include "OpenCLCU.h"

namespace OpenCL
{
    class Core
    {
        private:
            std::vector<cl::Platform> platforms;
            std::vector<ComputeUnit> computeUnits;

            cl::Program::Sources sources;

            void buildPlatforms();
            void buildComputeUnits();

        public:
            Core();

            static void showDevices();

            void addSourceFile(std::string fileName);
            void addKernel(const std::string& kernelName, uint nElements);
            void clearKernels();

            template<class T> void writeBuffer(std::vector<T>& hBuffer);
            template<class T> void readBuffer(std::vector<T>& hBuffer);
            template<class T> void syncDevicesBuffers(std::vector<T>& hBuffer);
            template<class T> void addArgument(std::string kernelName, std::vector<T>& hBuffer);

            void run();
    };

    template<class T> void Core::writeBuffer(std::vector<T>& hBuffer)
    {
        for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
            it->writeBuffer<T>(hBuffer);
        }

        for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
            it->waitFinish();
        }
    }

    template<class T> void Core::readBuffer(std::vector<T>& hBuffer)
    {
        for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
            it->readBuffer<T>(hBuffer);
        }

        for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
            it->waitFinish();
        }
    }

    template<class T> void Core::syncDevicesBuffers(std::vector<T>& hBuffer)
    {
        this->readBuffer<T>(hBuffer);
        this->writeBuffer<T>(hBuffer);
    }

    template<class T> void Core::addArgument(std::string kernelName, std::vector<T>& hBuffer)
    {
        for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
            it->addArgument(kernelName, hBuffer);
        }
    }
}

#endif // OPENCL_H
