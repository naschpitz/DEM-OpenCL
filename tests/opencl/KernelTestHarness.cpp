#include "KernelTestHarness.h"

#include <OCLW_Core.hpp>

namespace
{
    const std::string KERNEL_SOURCE_PATH = "../opencl/Tests.cpp.cl";

    const cl_float4 ZERO4 = {0.0f, 0.0f, 0.0f, 0.0f};
}

KernelTestHarness::KernelTestHarness()
{
    this->ready = false;

    try {
        this->core = std::make_unique<OpenCLWrapper::Core>(false);
        this->core->addSourceFile(KERNEL_SOURCE_PATH);
        this->ready = true;
    }

    catch (...) {
        this->core.reset();
        this->ready = false;
    }
}

KernelTestHarness::~KernelTestHarness() = default;

bool KernelTestHarness::isReady() const
{
    return this->ready;
}

cl_float4 KernelTestHarness::runSmokePassthrough(const cl_float4& input)
{
    std::vector<cl_float4> inputVec = { input };
    std::vector<cl_float4> outputVec = { ZERO4 };

    this->core->writeBuffer<cl_float4>("smoke_input", inputVec, 0);
    this->core->writeBuffer<cl_float4>("smoke_output", outputVec, 0);

    this->core->addKernel("test_smoke_passthrough", 1);
    this->core->addArgument<cl_float4>("test_smoke_passthrough", "smoke_input");
    this->core->addArgument<cl_float4>("test_smoke_passthrough", "smoke_output");

    this->core->run();
    this->core->clearKernels();

    this->core->readBuffer<cl_float4>("smoke_output", outputVec, 0);

    return outputVec[0];
}

std::vector<cl_float4> KernelTestHarness::runVectorGetUnitary(const std::vector<cl_float4>& inputs)
{
    std::vector<cl_float4> output(inputs.size(), ZERO4);

    this->core->writeBuffer<cl_float4>("vec_input", inputs, 0);
    this->core->writeBuffer<cl_float4>("vec_output", output, 0);

    this->core->addKernel("test_vector_getUnitary", inputs.size());
    this->core->addArgument<cl_float4>("test_vector_getUnitary", "vec_input");
    this->core->addArgument<cl_float4>("test_vector_getUnitary", "vec_output");

    this->core->run();
    this->core->clearKernels();

    this->core->readBuffer<cl_float4>("vec_output", output, 0);

    return output;
}

std::vector<cl_float4> KernelTestHarness::runEdgeGetClosestTo(const std::vector<VertexCL>& edgeVertices, const std::vector<cl_float4>& queries)
{
    std::vector<cl_float4> output(queries.size(), ZERO4);

    this->core->writeBuffer<VertexCL>("edge_verts", edgeVertices, 0);
    this->core->writeBuffer<cl_float4>("edge_queries", queries, 0);
    this->core->writeBuffer<cl_float4>("edge_output", output, 0);

    this->core->addKernel("test_edge_getClosestTo", queries.size());
    this->core->addArgument<VertexCL>("test_edge_getClosestTo", "edge_verts");
    this->core->addArgument<cl_float4>("test_edge_getClosestTo", "edge_queries");
    this->core->addArgument<cl_float4>("test_edge_getClosestTo", "edge_output");

    this->core->run();
    this->core->clearKernels();

    this->core->readBuffer<cl_float4>("edge_output", output, 0);

    return output;
}

void KernelTestHarness::runParticleGetClosestTo(const std::vector<ParticleCL>& particles, std::vector<cl_float4>& outThis, std::vector<cl_float4>& outOther)
{
    outThis.assign(particles.size() / 2, ZERO4);
    outOther.assign(particles.size() / 2, ZERO4);

    this->core->writeBuffer<ParticleCL>("part_particles", particles, 0);
    this->core->writeBuffer<cl_float4>("part_out_this", outThis, 0);
    this->core->writeBuffer<cl_float4>("part_out_other", outOther, 0);

    this->core->addKernel("test_particle_getClosestTo", particles.size() / 2);
    this->core->addArgument<ParticleCL>("test_particle_getClosestTo", "part_particles");
    this->core->addArgument<cl_float4>("test_particle_getClosestTo", "part_out_this");
    this->core->addArgument<cl_float4>("test_particle_getClosestTo", "part_out_other");

    this->core->run();
    this->core->clearKernels();

    this->core->readBuffer<cl_float4>("part_out_this", outThis, 0);
    this->core->readBuffer<cl_float4>("part_out_other", outOther, 0);
}

std::vector<cl_uint> KernelTestHarness::runParticleIsInternal(const std::vector<ParticleCL>& particles, const std::vector<cl_float4>& queries)
{
    std::vector<cl_uint> output(particles.size(), 0);

    this->core->writeBuffer<ParticleCL>("isint_particles", particles, 0);
    this->core->writeBuffer<cl_float4>("isint_queries", queries, 0);
    this->core->writeBuffer<cl_uint>("isint_output", output, 0);

    this->core->addKernel("test_particle_isInternal", particles.size());
    this->core->addArgument<ParticleCL>("test_particle_isInternal", "isint_particles");
    this->core->addArgument<cl_float4>("test_particle_isInternal", "isint_queries");
    this->core->addArgument<cl_uint>("test_particle_isInternal", "isint_output");

    this->core->run();
    this->core->clearKernels();

    this->core->readBuffer<cl_uint>("isint_output", output, 0);

    return output;
}
