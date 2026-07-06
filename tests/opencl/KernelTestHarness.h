#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 300
#endif

#ifndef KERNELTESTHARNESS_H
#define KERNELTESTHARNESS_H

#include <CL/opencl.hpp>

#include "Face.h"
#include "Material.h"
#include "Particle.h"
#include "Vertex.h"

#include <memory>
#include <string>
#include <vector>

namespace OpenCLWrapper
{
  class Core;
}

class KernelTestHarness
{
  private:
    std::unique_ptr<OpenCLWrapper::Core> core;
    bool ready;

  public:
    KernelTestHarness();
    ~KernelTestHarness();

    bool isReady() const;

    cl_float4 runSmokePassthrough(const cl_float4& input);

    std::vector<cl_float4> runVectorGetUnitary(const std::vector<cl_float4>& inputs);

    std::vector<cl_float4> runEdgeGetClosestTo(const std::vector<VertexCL>& edgeVertices,
                                               const std::vector<cl_float4>& queries);

    void runParticleGetClosestTo(const std::vector<ParticleCL>& particles, std::vector<cl_float4>& outThis,
                                 std::vector<cl_float4>& outOther);

    std::vector<cl_uint> runParticleIsInternal(const std::vector<ParticleCL>& particles,
                                               const std::vector<cl_float4>& queries);

    void runFaceGetClosestTo(const std::vector<FaceCL>& faces, const std::vector<ParticleCL>& particles,
                             std::vector<cl_float4>& outFace, std::vector<cl_float4>& outParticle);

    void runObjectToParticle(const std::vector<FaceCL>& faces, const std::vector<ParticleCL>& particles,
                             std::vector<cl_float4>& outFace, std::vector<cl_float4>& outParticle);

    cl_float4 runMaterialCalculateForce(const MaterialCL& material, cl_float4 distance, cl_float4 distanceUnitary,
                                        bool internal, float contactArea, float originalLength, cl_float4 oldForce);

    cl_float4 runMaterialCalculateDragForce(const MaterialCL& material, cl_float4 velocity, cl_float4 rotationVelocity,
                                            cl_float4 force);

    void runParticleToParticleWorker(ParticleCL& thisParticle, const ParticleCL& otherParticle,
                                     const MaterialCL& material, cl_float4& outForce, cl_float4& outTorque);

    void runParticleToFaceWorker(ParticleCL& thisParticle, const FaceCL& otherFace, const MaterialCL& material,
                                 cl_float4& outForce, cl_float4& outTorque);
};

#endif // KERNELTESTHARNESS_H
