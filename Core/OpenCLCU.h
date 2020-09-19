#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#ifndef OPENCLCU_H
#define OPENCLCU_H

#include <CL/cl2.hpp>
#include <iostream>
#include <unistd.h>
#include <map>

#include "OpenCLKernel.h"

namespace OpenCL
{
    class ComputeUnit
    {
        public:
            ComputeUnit();
            ComputeUnit(const cl::Device& device, uint index, double fraction, bool last);

        private:
            uint index;
            double fraction;
            bool last;

            bool programBuilt = false;

            cl::Device device;
            cl::Context context;
            cl::CommandQueue queue;
            cl::Program program;
            std::vector<Kernel> kernels;

            cl::Program::Sources sources;

            std::map<void*, cl::Buffer> bufferMap;

            void buildContext();
            void buildQueue();
            void buildProgram();

        public:
            void addSource(const std::string& sourceCode);
            void addKernel(const std::string& kernelName, uint nElements);
            void clearKernels();

            template<class T> void writeBuffer(std::vector<T>& hBuffer);
            template<class T> void readBuffer(std::vector<T>& hBuffer);
            template<class T> void addArgument(const std::string& kernelName, std::vector<T>& hBuffer);

            void run();
            void waitFinish();
    };

    template<class T> void ComputeUnit::writeBuffer(std::vector<T> &hBuffer)
    {
        std::map<void*, cl::Buffer>::const_iterator it = this->bufferMap.find(&hBuffer);
        cl::Buffer dBuffer;

        if(it != this->bufferMap.end())
            dBuffer = it->second;

        else {
            dBuffer = cl::Buffer(this->context, CL_MEM_READ_WRITE, sizeof(T) * hBuffer.size());
            this->bufferMap[&hBuffer] = dBuffer;
        }

        this->queue.enqueueWriteBuffer(dBuffer, CL_FALSE, 0, sizeof(T) * hBuffer.size(), hBuffer.data());
    }

    template<class T> void ComputeUnit::readBuffer(std::vector<T>& hBuffer)
    {
        cl::Buffer dBuffer = this->bufferMap[&hBuffer];

        uint count = hBuffer.size() * this->fraction;
        uint offset = this->index * count;

        if(this->last)
            count = hBuffer.size() - offset;

        this->queue.enqueueReadBuffer(dBuffer, CL_TRUE, sizeof(T) * offset, sizeof(T) * count, hBuffer.data() + offset);
    }

    template<class T> void ComputeUnit::addArgument(const std::string& kernelName, std::vector<T>& hBuffer)
    {
        cl::Buffer dBuffer = this->bufferMap[&hBuffer];

        for(std::vector<Kernel>::iterator it = this->kernels.begin(); it != this->kernels.end(); it++) {
            if(it->name.compare(kernelName) == 0) {
                it->kernel.setArg(it->argsCount, dBuffer);
                it->argsCount++;
            }
        }
    }
}

#endif // OPENCLCU_H
