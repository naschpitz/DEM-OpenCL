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
    if (!this->programBuilt)
        this->buildProgram();

    // Alternative way to run the kernel.
    Kernel kernel;
    kernel.name = kernelName;
    kernel.nElements = nElements;

    cl_int result;

    kernel.kernel = cl::Kernel(this->program, kernelName.c_str(), &result);

    if (result != CL_SUCCESS) {
        std::cout << "Error creating kernel: " << result << "\n";
        exit(1);
    }

    this->kernels.push_back(kernel);
}

void Core::buildPlatform()
{
    // Get all platforms (drivers).
    cl::Platform::get(&(this->platforms));

    if(this->platforms.size() == 0) {
        std::cout << "No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    this->platform = this->platforms[0];
}

void Core::buildDevice()
{
    // Get default device of the default platform.
    this->platform.getDevices(CL_DEVICE_TYPE_ALL, &(this->devices));

    if(this->devices.size() == 0) {
        std::cout << "No devices found. Check OpenCL installation!\n";
        exit(1);
    }

    this->device = this->devices[0];
}

void Core::buildContext()
{
    cl_int result;

    this->context = cl::Context({this->device}, NULL, NULL, NULL, &result);

    if (result != CL_SUCCESS) {
        std::cout << "Error creating context: " << result << "\n";
        exit(1);
    }
}

void Core::buildQueue()
{
    // Create queue to which we will push commands for the device.
    this->queue = cl::CommandQueue(this->context, this->device);
}

void Core::buildProgram()
{
    cl_int result;

    this->program = cl::Program(this->context, this->sources, &result);

    if(result != CL_SUCCESS) {
        std::cout << "Error creating program: " << result << "\n";
        exit(1);
    }

    if(this->program.build({this->device}) != CL_SUCCESS) {
        std::cout << "Error building: " << this->program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->device) << "\n";
        exit(1);
    }

    this->programBuilt = true;
}

void Core::clearKernels()
{
    this->kernels.clear();
}

void Core::run()
{
    for(std::vector<Kernel>::iterator it = this->kernels.begin(); it != this->kernels.end(); it++) {
        cl_int result = this->queue.enqueueNDRangeKernel(it->kernel, cl::NullRange, cl::NDRange(it->nElements), cl::NullRange);

        if(result != CL_SUCCESS) {
            std::cout << "Error enqueueing kernel: " << result << "\n";
            exit(1);
        }

        this->queue.enqueueBarrierWithWaitList();
    }

    this->queue.finish();
}
