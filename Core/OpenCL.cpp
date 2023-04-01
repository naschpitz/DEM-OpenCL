#include "OpenCL.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace OpenCL;

Core::Core()
{
    this->showDevices();
    this->buildPlatforms();
    this->buildComputeUnits();
}

void Core::showDevices()
{
    std::vector<cl::Platform> platforms;

    cl::Platform::get(&(platforms));

    if(platforms.size() == 0) {
        std::cout << "No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    for(uint i = 0; i < platforms.size(); i++) {
        std::cout << "Platform: " << platforms[i].getInfo<CL_PLATFORM_NAME>() << "\n";

        std::vector<cl::Device> devices;

        platforms[i].getDevices(CL_DEVICE_TYPE_GPU, &(devices));

        if(devices.size() == 0) {
            std::cout << "      No devices found. Check OpenCL installation!\n";
            exit(1);
        }

        for(uint j = 0; j < devices.size(); j++) {
            std::cout << "      Device: " << devices[j].getInfo<CL_DEVICE_NAME>() << "\n";
        }
    }

    std::cout.flush();
}

void Core::buildPlatforms()
{
    std::cout << "Building platform...";

    // Get all platforms (drivers).
    cl::Platform::get(&(this->platforms));

    if(this->platforms.size() == 0) {
        std::cout << "No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    std::cout << " Done!\n";
}

void Core::buildComputeUnits()
{
    std::cout << "Building compute units...";

    std::vector<cl::Device> allDevices;

    for(std::vector<cl::Platform>::iterator it = this->platforms.begin(); it != this->platforms.end(); it++) {
        std::vector<cl::Device> devices;

        it->getDevices(CL_DEVICE_TYPE_GPU, &(devices));

        if(devices.size() == 0) {
            std::cout << "No devices found. Check OpenCL installation!\n";
            exit(1);
        }

        allDevices.insert(allDevices.end(), devices.begin(), devices.end());
    }

    for(std::vector<cl::Device>::iterator it = allDevices.begin(); it != allDevices.end(); it++) {
        uint index = std::distance(allDevices.begin(), it);
        double fraction = 1. / allDevices.size();
        bool last = false;

        if(std::distance(it, allDevices.end()) == 1)
            last = true;

        ComputeUnit computeUnit = ComputeUnit(*it, index, fraction, last);

        this->computeUnits.push_back(computeUnit);
    }

    std::cout << " Done!\n";
}

void Core::addSourceFile(std::string fileName)
{
    std::cout << "Reading source file...";

    std::ifstream sourceFile;
    sourceFile.open(fileName); // Open the input file

    if((sourceFile.rdstate() & std::ifstream::failbit) != 0) {
        std::cout << "Source file not found.\n";
        exit(1);
    }

    std::stringstream stream;
    stream << sourceFile.rdbuf(); // Read the file.
    std::string sourceCode = stream.str(); //str holds the content of the file.

    for(std::vector<ComputeUnit>::iterator it = this->computeUnits.begin(); it != this->computeUnits.end(); it++) {
        it->addSource(sourceCode);
    }

    std::cout << " Done!\n";
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
