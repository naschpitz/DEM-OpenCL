#include "OpenCL.h"

#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace OpenCL;

std::mutex Core::mutex;
std::vector<cl::Platform> Core::platforms;
std::vector<cl::Device> Core::devices;
std::map<const cl::Device*, uint> Core::devicesUsage;

Core::Core(bool useMultipleGPUs)
{
    this->useMultipleGPUs = useMultipleGPUs;

    this->buildComputeUnits();
}

Core::~Core()
{
    for(auto it = this->devicesInUse.begin(); it != this->devicesInUse.end(); it++) {
        Core::removeJobFromDevice(**it);
    }
}

void Core::buildComputeUnits()
{
    std::cout << "Building compute units...\n";

    if(this->useMultipleGPUs) {
        std::cout << "      Using all available GPUs.\n";

        for(std::vector<cl::Device>::iterator it = Core::devices.begin(); it != Core::devices.end(); it++) {
            uint index = std::distance(Core::devices.begin(), it);
            double fraction = 1. / Core::devices.size();
            bool last = false;

            if(std::distance(it, Core::devices.end()) == 1)
                last = true;

            ComputeUnit computeUnit = ComputeUnit(*it, index, fraction, last);

            Core::addJobToDevice(*it);

            this->computeUnits.push_back(computeUnit);
        }
    }

    else {
        Core::mutex.lock();

        const cl::Device& device = Core::getAvailableDevice();

        auto itFirst = Core::devices.begin();
        auto itLast = Core::devices.end();
        auto it = std::find(itFirst, itLast, device);

        if(it == itLast) {
            std::cout << "      Device not found\n";
            exit(1);
        }

        uint deviceIndex = std::distance(itFirst, it);

        std::cout << "      Using GPU #" << deviceIndex << "\n";

        uint index = 0;
        double fraction = 1;
        bool last = true;

        ComputeUnit computeUnit = ComputeUnit(device, index, fraction, last);

        Core::addJobToDevice(device);

        this->computeUnits.push_back(computeUnit);

        Core::mutex.unlock();
    }

    std::cout << " Done!\n";
    std::cout.flush();
}

void Core::addJobToDevice(const cl::Device& device)
{
    this->devicesInUse.push_back(&device);

    Core::devicesUsage[&device]++;
}

void Core::removeJobFromDevice(const cl::Device& device)
{
    Core::devicesUsage[&device]--;
}

void Core::addSourceFile(std::string fileName)
{
    std::cout << "Reading source file...\n";

    std::ifstream sourceFile;
    sourceFile.open(fileName); // Open the input file

    if((sourceFile.rdstate() & std::ifstream::failbit) != 0) {
        std::cout << "      Source file not found.\n";
        exit(1);
    }

    std::stringstream stream;
    stream << sourceFile.rdbuf(); // Read the file.
    std::string sourceCode = stream.str(); //str holds the content of the file.

    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
        it->addSource(sourceCode);
    }

    std::cout << " Done!\n";
    std::cout.flush();
}

void Core::addKernel(const std::string& kernelName, uint nElements)
{
    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
        it->addKernel(kernelName, nElements);
    }
}

void Core::clearKernels()
{
    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
        it->clearKernels();
    }
}

void Core::run()
{
    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
        it->run();
    }

    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
        it->waitFinish();
    }
}

void Core::buildPlatforms()
{
    std::cout << "Building platform...\n";

    // Get all platforms (drivers).
    cl::Platform::get(&(Core::platforms));

    if(Core::platforms.size() == 0) {
        std::cout << "      No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    std::cout << " Done!\n";
    std::cout.flush();
}

void Core::buildDevices()
{
    std::cout << "Building devices...\n";

    for(std::vector<cl::Platform>::iterator it = Core::platforms.begin(); it != Core::platforms.end(); it++) {
        std::cout << "Platform: " << it->getInfo<CL_PLATFORM_NAME>() << "\n";

        it->getDevices(CL_DEVICE_TYPE_GPU, &(Core::devices));

        if(Core::devices.size() == 0) {
            std::cout << "      No devices found. Check OpenCL installation!\n";
            exit(1);
        }

        for(std::vector<cl::Device>::iterator it = Core::devices.begin(); it != Core::devices.end(); it++) {
            std::cout << "      Device: " << it->getInfo<CL_DEVICE_NAME>() << "\n";
        }
    }

    std::cout << " Done!\n";
    std::cout.flush();
}

void Core::buildDevicesUsageMap()
{
    for(std::vector<cl::Device>::iterator it = Core::devices.begin(); it != Core::devices.end(); it++) {
        Core::devicesUsage[&*it] = 0;
    }
}

const cl::Device& Core::getAvailableDevice()
{
    const cl::Device* deviceWithLeastUsage = nullptr;

    uint leastUsage = UINT_MAX;

    // In reverse order, so it will prioritize the latest GPU, which is not connected to the video output.
    for(auto it = Core::devicesUsage.rbegin(); it != Core::devicesUsage.rend(); it++) {
        if(it->second < leastUsage) {
          deviceWithLeastUsage = it->first;
          leastUsage = it->second;
        }
    }

    return *deviceWithLeastUsage;
}

void Core::printDevicesInfo()
{
    std::cout << "Devices Information:\n";

    for(std::vector<cl::Device>::iterator it = Core::devices.begin(); it != Core::devices.end(); it++) {
        std::string deviceName = it->getInfo<CL_DEVICE_NAME>();
        std::string deviceVendor = it->getInfo<CL_DEVICE_VENDOR>();
        std::string driverVersion = it->getInfo<CL_DRIVER_VERSION>();
        std::string deviceVersion = it->getInfo<CL_DEVICE_VERSION>();
        cl_ulong globalMemSize = it->getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
        cl_ulong localMemSize = it->getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
        cl_uint computeUnits = it->getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
        size_t maxWorkGroupSize = it->getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

        std::cout << "  Device Name: " << deviceName << "\n";
        std::cout << "  Vendor: " << deviceVendor << "\n";
        std::cout << "  Driver Version: " << driverVersion << "\n";
        std::cout << "  Device Version: " << deviceVersion << "\n";
        std::cout << "  Global Memory Size: " << globalMemSize / (1024 * 1024) << " MB\n";
        std::cout << "  Local Memory Size: " << localMemSize / 1024 << " KB\n";
        std::cout << "  Compute Units: " << computeUnits << "\n";
        std::cout << "  Max Work Group Size: " << maxWorkGroupSize << "\n";
        std::cout << "------------------------------------\n";
    }

    std::cout.flush();
}

void Core::initialize() {
    Core::buildPlatforms();
    Core::buildDevices();
    Core::buildDevicesUsageMap();
    Core::printDevicesInfo();
}

std::map<const cl::Device*, uint>& Core::getDevicesUsage() {
    return Core::devicesUsage;
}
