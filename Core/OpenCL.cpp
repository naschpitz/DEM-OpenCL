#include "OpenCL.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace OpenCL;

Core::Core()
{
    this->programBuilt = false;

    this->showDevices();
    this->buildPlatform();
    this->buildDevice();
    this->buildContext();
    this->buildQueue();
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

        platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &(devices));

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

void Core::addSourceFile(std::string fileName)
{
    std::ifstream sourceFile;
    sourceFile.open(fileName); // Open the input file

    if((sourceFile.rdstate() & std::ifstream::failbit) != 0) {
        std::cout << "Source file not found.\n";
        exit(1);
    }

    std::stringstream stream;
    stream << sourceFile.rdbuf(); // Read the file.
    std::string sourceCode = stream.str(); //str holds the content of the file.

    this->sources.push_back({sourceCode.c_str(), sourceCode.length()});
}

void Core::addKernel(std::string kernelName, uint nElements)
{
    if(!this->programBuilt)
        this->buildProgram();

    // Alternative way to run the kernel.
    Kernel kernel;
    kernel.name = kernelName;
    kernel.nElements = nElements;

    cl_int result;

    kernel.kernel = cl::Kernel(this->program, kernelName.c_str(), &result);

    if(result != CL_SUCCESS) {
        std::cout << "Error creating kernel: " << result << "\n";
        exit(1);
    }

    this->kernels.push_back(kernel);
}

void Core::buildPlatform()
{
    std::cout << "Building platform...";

    // Get all platforms (drivers).
    cl::Platform::get(&(this->platforms));

    if(this->platforms.size() == 0) {
        std::cout << "No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    this->platform = this->platforms[0];

    std::cout << " Done!\n";
}

void Core::buildDevice()
{
    std::cout << "Building devices...";

    // Get default device of the default platform.
    this->platform.getDevices(CL_DEVICE_TYPE_ALL, &(this->devices));

    if(this->devices.size() == 0) {
        std::cout << "No devices found. Check OpenCL installation!\n";
        exit(1);
    }

    std::cout << " Done!\n";
}

void Core::buildContext()
{
    std::cout << "Building context...";

    cl_int result;

    this->context = cl::Context(this->devices, NULL, NULL, NULL, &result);

    if(result != CL_SUCCESS) {
        std::cout << "Error creating context: " << result << "\n";
        exit(1);
    }

    std::cout << " Done!\n";
}

void Core::buildQueue()
{
    std::cout << "Building queue...";

    // Create queues to which we will push commands for the devices.
    for(std::vector<cl::Device>::iterator it = this->devices.begin(); it != this->devices.end(); it++) {
        this->queues.push_back(cl::CommandQueue(this->context, *it));
    }

    std::cout << " Done!\n";
}

void Core::buildProgram()
{
    std::cout << "Building program...";

    cl_int result;

    this->program = cl::Program(this->context, this->sources, &result);

    if(result != CL_SUCCESS) {
        std::cout << "Error creating program: " << result << "\n";
        exit(1);
    }

    if(this->program.build(this->devices, "-I ./") != CL_SUCCESS) {
        for(std::vector<cl::Device>::iterator it = this->devices.begin(); it != this->devices.end(); it++) {
            std::cout << "Error building: " << this->program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(*it) << "\n";
        }

        exit(1);
    }

    this->programBuilt = true;

    std::cout << " Done!\n";
}

void Core::clearKernels()
{
    this->kernels.clear();
}

void Core::run()
{
    for(std::vector<cl::CommandQueue>::iterator queueIt = this->queues.begin(); queueIt != this->queues.end(); queueIt++) {
        for(std::vector<Kernel>::iterator kernelIt = this->kernels.begin(); kernelIt != this->kernels.end(); kernelIt++) {
            uint countPerQueue = kernelIt->nElements / this->queues.size();
            uint offset = std::distance(this->queues.begin(), queueIt) * countPerQueue;

            cl_int result = queueIt->enqueueNDRangeKernel(kernelIt->kernel, offset, cl::NDRange(countPerQueue), cl::NullRange);

            if(result != CL_SUCCESS) {
                std::cout << "Error enqueueing kernel: " << result << "\n";
                exit(1);
            }

            queueIt->enqueueBarrierWithWaitList();
        }
    }

    for(std::vector<cl::CommandQueue>::iterator queueIt = this->queues.begin(); queueIt != this->queues.end(); queueIt++) {
        queueIt->finish();
    }
}
