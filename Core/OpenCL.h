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
#include <map>

namespace OpenCL
{
    struct Kernel
    {
        std::string name;
        cl_uint nElements;
        cl::Kernel kernel;
        uint argsCount = 0;
    };

    class Core
    {
        private:
            std::vector<cl::Platform> platforms;
            cl::Platform platform;

            std::vector<cl::Device> devices;
            cl::Device device;

            cl::Context context;

            cl::Program::Sources sources;

            bool programBuilt;
            cl::Program program;

            cl::CommandQueue queue;

            std::vector<Kernel> kernels;

            std::map<void*, cl::Buffer> bufferMap;

            void buildPlatform();
            void buildDevice();
            void buildContext();
            void buildQueue();
            void buildSource(std::string fileName);
            void buildProgram();

        public:
            Core();

            static void showDevices();

            void addSourceFile(std::string fileName);
            void addKernel(std::string kernelName, uint nElements);
            void clearKernels();

            template<class T> void writeBuffer(std::vector<T> &hBuffer);
            template<class T> void readBuffer(std::vector<T> &hBuffer);
            template<class T> void copyBuffer(std::vector<T> &hSourceBuffer, std::vector<T> &hDestinationBuffer);
            template<class T> void addArgument(std::string kernelName, std::vector<T> &hBuffer);

            void run();
    };

    template<class T> void Core::writeBuffer(std::vector<T> &hBuffer)
    {
        cl::Buffer dBuffer(this->context, CL_MEM_READ_WRITE, sizeof(T) * hBuffer.size());
        this->bufferMap[&hBuffer] = dBuffer;

        this->queue.enqueueWriteBuffer(dBuffer, CL_TRUE, 0, sizeof(T) * hBuffer.size(), hBuffer.data());
    }

    template<class T> void Core::readBuffer(std::vector<T> &hBuffer)
    {
        cl::Buffer dBuffer = this->bufferMap[&hBuffer];

        this->queue.enqueueReadBuffer(dBuffer, CL_TRUE, 0, sizeof(T) * hBuffer.size(), hBuffer.data());
    }

    template<class T> void Core::copyBuffer(std::vector<T> &hSourceBuffer, std::vector<T> &hDestinationBuffer)
    {
        cl::Buffer dSourceBuffer = this->bufferMap[&hSourceBuffer];
        cl::Buffer dDestinationBuffer = this->bufferMap[&hDestinationBuffer];

        cl::Event event;
        this->queue.enqueueCopyBuffer(dSourceBuffer, dDestinationBuffer, 0, 0, sizeof(T) * hSourceBuffer.size(), __null, &event);
        this->queue.enqueueBarrierWithWaitList(0, &event);
    }

    template<class T> void Core::addArgument(std::string kernelName, std::vector<T> &hBuffer)
    {
        cl::Buffer dBuffer = this->bufferMap[&hBuffer];

        for (std::vector<Kernel>::iterator it = this->kernels.begin(); it != this->kernels.end(); it++) {
            if(it->name.compare(kernelName) == 0) {
                it->kernel.setArg(it->argsCount, dBuffer);
                it->argsCount++;
            }
        }
    }
}

#endif // OPENCL_H
