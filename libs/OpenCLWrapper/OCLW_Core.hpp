#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 300
#endif

#ifndef OCLW_CORE_H
#define OCLW_CORE_H

#include <CL/opencl.hpp>
#include <mutex>
#include <unistd.h>

#include "OCLW_CU.hpp"

namespace OpenCLWrapper
{
  class Core
  {
    private:
      std::vector<ComputeUnit> computeUnits;

      cl::Program::Sources sources;

      bool useMultipleGPUs;
      std::vector<const cl::Device*> devicesInUse;

      void buildComputeUnits();

      void addJobToDevice(const cl::Device& device);
      void removeJobFromDevice(const cl::Device& device);

      static std::mutex mutex;

      static std::vector<cl::Platform> platforms;
      static std::vector<cl::Device> devices;
      static std::map<const cl::Device*, uint> devicesUsage;

      static void buildPlatforms();
      static void buildDevices();
      static void buildDevicesUsageMap();

      static const cl::Device& getAvailableDevice();
      static void printDevicesInfo();

    public:
      Core(bool useMultipleGPUs = false);
      ~Core();

      void addSourceFile(std::string fileName);
      void addKernel(const std::string& kernelName, ulong nElements, ulong offset = 0);
      void clearKernels();

      template<class T> void allocateBuffer(const std::string& name, ulong size);

      template<class T> void writeBuffer(const std::vector<T>& hBuffer);
      template<class T> void writeBuffer(const std::string& name, const std::vector<T>& hBuffer, ulong dBufferOffset);

      template<class T> void readBuffer(std::vector<T>& hBuffer);
      template<class T> void readBuffer(const std::string& name, std::vector<T>& hBuffer, ulong dBufferOffset);

      template<class T> void syncDevicesBuffers(std::vector<T>& hBuffer);

      template<class T> void addArgument(const std::string& kernelName, const std::string& bufferName);
      template<class T> void addArgument(const std::string& kernelName, const std::vector<T>& hBuffer);
      template<class T> void addArgument(const std::string& kernelName, const T& variable);

      void run();

      static void initialize();
      static std::map<const cl::Device*, uint>& getDevicesUsage();
  };

  template<class T> void Core::allocateBuffer(const std::string& name, ulong size)
  {
    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
      it->allocateBuffer<T>(name, size);
    }

    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
      it->waitFinish();
    }
  }

  template<class T> void Core::writeBuffer(const std::vector<T>& hBuffer)
  {
    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
      it->writeBuffer<T>(hBuffer);
    }

    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
      it->waitFinish();
    }
  }

  template<class T> void Core::writeBuffer(const std::string& name, const std::vector<T>& hBuffer, ulong dBufferOffset) {
    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
      it->writeBuffer<T>(name, hBuffer, dBufferOffset);
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

  template<class T> void Core::readBuffer(const std::string& name, std::vector<T>& hBuffer, ulong dBufferOffset)
  {
    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
      it->readBuffer<T>(name, hBuffer, dBufferOffset);
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

  template<class T> void Core::addArgument(const std::string& kernelName, const std::string& bufferName)
  {
    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
      it->addArgument<T>(kernelName, bufferName);
    }
  }

  template<class T> void Core::addArgument(const std::string& kernelName, const std::vector<T>& hBuffer)
  {
    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
      it->addArgument(kernelName, hBuffer);
    }
  }

  template<class T> void Core::addArgument(const std::string& kernelName, const T& variable)
  {
    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
      it->addArgument(kernelName, variable);
    }
  }
}

#endif // OCLW_CORE_H
