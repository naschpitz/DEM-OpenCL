#include "OpenCLCU.h"

using namespace OpenCL;

ComputeUnit::ComputeUnit()
{
    this->programBuilt = false;
}

ComputeUnit::ComputeUnit(const cl::Device& device, uint index, double fraction, bool last)
{
    this->programBuilt = false;

    this->device = device;
    this->index = index;
    this->fraction = fraction;
    this->last = last;

    this->buildContext();
    this->buildQueue();
}

void ComputeUnit::buildContext()
{
    std::cout << "Building context...";

    cl_int result;

    this->context = cl::Context(this->device, NULL, NULL, NULL, &result);

    if(result != CL_SUCCESS) {
        std::cout << "Error creating context: " << result << "\n";
        exit(1);
    }

    std::cout << " Done!\n";
}

void ComputeUnit::buildQueue()
{
    std::cout << "Building queue...";

    this->queue = cl::CommandQueue(this->context, this->device);

    std::cout << " Done!\n";
}

void ComputeUnit::buildProgram()
{
    std::cout << "Building program...";

    cl_int result;

    this->program = cl::Program(this->context, this->sources, &result);

    if(result != CL_SUCCESS) {
        std::cout << "Error creating program: " << result << "\n";
        exit(1);
    }

    if(this->program.build({this->device}, "-I ./") != CL_SUCCESS) {
        std::cout << "Error building: " << this->program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->device) << "\n";
        std::cout.flush();
        exit(1);
    }

    this->programBuilt = true;

    std::cout << " Done!\n";
}

void ComputeUnit::addSource(const std::string& sourceCode)
{
    this->sources.push_back({sourceCode.c_str(), sourceCode.length()});
}

void ComputeUnit::addKernel(const std::string& kernelName, uint nElements)
{
    if(!this->programBuilt)
        this->buildProgram();

    // Alternative way to run the kernel.
    Kernel kernel;
    kernel.name = kernelName;
    kernel.nElements = nElements;

    std::cout << "Building kernel...";

    cl_int result;
    kernel.kernel = cl::Kernel(this->program, kernelName.c_str(), &result);

    if(result != CL_SUCCESS) {
        std::cout << "Error creating kernel: " << result << "\n";
        exit(1);
    }

    this->kernels.push_back(kernel);

    std::cout << " Done!\n";
}

void ComputeUnit::clearKernels()
{
    this->kernels.clear();
}

void ComputeUnit::run()
{
    for(std::vector<Kernel>::iterator it = this->kernels.begin(); it != this->kernels.end(); it++) {
        uint count = it->nElements * this->fraction;
        uint offset = this->index * count;

        if(this->last)
            count = it->nElements - offset;

        cl_int result = this->queue.enqueueNDRangeKernel(it->kernel, offset, cl::NDRange(count), cl::NullRange);

        if(result != CL_SUCCESS) {
            std::cout << "Error enqueueing kernel: " << result << "\n";
            exit(1);
        }

        this->queue.enqueueBarrier();
    }

    this->queue.flush();
}

void ComputeUnit::waitFinish()
{
    this->queue.finish();
}
