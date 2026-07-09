#include "KernelTestHarness.h"

#include <OCLW_Core.hpp>

#include <limits>

namespace
{
  const std::string KERNEL_SOURCE_PATH = "../tests/opencl/Tests.cpp.cl";
  const std::string SIMULATION_SOURCE_PATH = "../opencl/Simulation.cpp.cl";

  const cl_float4 ZERO4 = {0.0f, 0.0f, 0.0f, 0.0f};
}

KernelTestHarness::KernelTestHarness()
{
  this->ready = false;

  try {
    this->core = std::make_unique<OpenCLWrapper::Core>(false);
    this->core->addSourceFile(KERNEL_SOURCE_PATH);
    this->core->addSourceFile(SIMULATION_SOURCE_PATH);
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
  std::vector<cl_float4> inputVec = {input};
  std::vector<cl_float4> outputVec = {ZERO4};

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

std::vector<cl_float4> KernelTestHarness::runEdgeGetClosestTo(const std::vector<VertexCL>& edgeVertices,
                                                              const std::vector<cl_float4>& queries)
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

void KernelTestHarness::runParticleGetClosestTo(const std::vector<ParticleCL>& particles,
                                                std::vector<cl_float4>& outThis, std::vector<cl_float4>& outOther)
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

std::vector<cl_uint> KernelTestHarness::runParticleIsInternal(const std::vector<ParticleCL>& particles,
                                                              const std::vector<cl_float4>& queries)
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

void KernelTestHarness::runFaceGetClosestToParticle(const std::vector<FaceCL>& faces,
                                                    const std::vector<ParticleCL>& particles,
                                                    std::vector<cl_float4>& outFace,
                                                    std::vector<cl_float4>& outParticle)
{
  outFace.assign(faces.size(), ZERO4);
  outParticle.assign(faces.size(), ZERO4);

  this->core->writeBuffer<FaceCL>("face_faces", faces, 0);
  this->core->writeBuffer<ParticleCL>("face_particles", particles, 0);
  this->core->writeBuffer<cl_float4>("face_out_face", outFace, 0);
  this->core->writeBuffer<cl_float4>("face_out_particle", outParticle, 0);

  this->core->addKernel("test_face_getClosestToParticle", faces.size());
  this->core->addArgument<FaceCL>("test_face_getClosestToParticle", "face_faces");
  this->core->addArgument<ParticleCL>("test_face_getClosestToParticle", "face_particles");
  this->core->addArgument<cl_float4>("test_face_getClosestToParticle", "face_out_face");
  this->core->addArgument<cl_float4>("test_face_getClosestToParticle", "face_out_particle");

  this->core->run();
  this->core->clearKernels();

  this->core->readBuffer<cl_float4>("face_out_face", outFace, 0);
  this->core->readBuffer<cl_float4>("face_out_particle", outParticle, 0);
}

void KernelTestHarness::runObjectToParticle(const std::vector<FaceCL>& faces, const std::vector<ParticleCL>& particles,
                                            std::vector<cl_float4>& outFace, std::vector<cl_float4>& outParticle)
{
  cl_uint numFaces = static_cast<cl_uint>(faces.size());

  outFace.assign(particles.size(), ZERO4);
  outParticle.assign(particles.size(), ZERO4);

  this->core->writeBuffer<FaceCL>("obj_faces", faces, 0);
  this->core->writeBuffer<ParticleCL>("obj_particles", particles, 0);
  this->core->writeBuffer<cl_float4>("obj_out_face", outFace, 0);
  this->core->writeBuffer<cl_float4>("obj_out_particle", outParticle, 0);

  this->core->addKernel("test_object_to_particle", particles.size());
  this->core->addArgument<FaceCL>("test_object_to_particle", "obj_faces");
  this->core->addArgument<cl_uint>("test_object_to_particle", numFaces);
  this->core->addArgument<ParticleCL>("test_object_to_particle", "obj_particles");
  this->core->addArgument<cl_float4>("test_object_to_particle", "obj_out_face");
  this->core->addArgument<cl_float4>("test_object_to_particle", "obj_out_particle");

  this->core->run();
  this->core->clearKernels();

  this->core->readBuffer<cl_float4>("obj_out_face", outFace, 0);
  this->core->readBuffer<cl_float4>("obj_out_particle", outParticle, 0);
}

void KernelTestHarness::runCalculateFaceToFace(std::vector<FaceCL>& faces, const MaterialsManagerCL& materialsManager)
{
  std::vector<MaterialsManagerCL> mmVec = {materialsManager};

  this->core->writeBuffer<FaceCL>("ftf_faces", faces, 0);
  this->core->writeBuffer<MaterialsManagerCL>("ftf_mm", mmVec, 0);

  this->core->addKernel("calculate_face_to_face", faces.size());
  this->core->addArgument<FaceCL>("calculate_face_to_face", "ftf_faces");
  this->core->addArgument<MaterialsManagerCL>("calculate_face_to_face", "ftf_mm");

  this->core->run();
  this->core->clearKernels();

  this->core->readBuffer<FaceCL>("ftf_faces", faces, 0);
}

cl_float4 KernelTestHarness::runMaterialCalculateForce(const MaterialCL& material, cl_float4 distance,
                                                       cl_float4 distanceUnitary, bool internal, float contactArea,
                                                       float originalLength, cl_float4 oldForce)
{
  std::vector<MaterialCL> materials = {material};
  std::vector<cl_float4> output = {ZERO4};

  this->core->writeBuffer<MaterialCL>("mat_materials", materials, 0);
  this->core->writeBuffer<cl_float4>("mat_output", output, 0);

  this->core->addKernel("test_material_calculateForce", 1);
  this->core->addArgument<MaterialCL>("test_material_calculateForce", "mat_materials");
  this->core->addArgument<cl_float4>("test_material_calculateForce", distance);
  this->core->addArgument<cl_float4>("test_material_calculateForce", distanceUnitary);
  this->core->addArgument<int>("test_material_calculateForce", static_cast<int>(internal));
  this->core->addArgument<float>("test_material_calculateForce", contactArea);
  this->core->addArgument<float>("test_material_calculateForce", originalLength);
  this->core->addArgument<cl_float4>("test_material_calculateForce", oldForce);
  this->core->addArgument<cl_float4>("test_material_calculateForce", "mat_output");

  this->core->run();
  this->core->clearKernels();

  this->core->readBuffer<cl_float4>("mat_output", output, 0);

  return output[0];
}

cl_float4 KernelTestHarness::runMaterialCalculateDragForce(const MaterialCL& material, cl_float4 velocity,
                                                           cl_float4 rotationVelocity, cl_float4 force)
{
  std::vector<MaterialCL> materials = {material};
  std::vector<cl_float4> output = {ZERO4};

  this->core->writeBuffer<MaterialCL>("mat_materials", materials, 0);
  this->core->writeBuffer<cl_float4>("mat_output", output, 0);

  this->core->addKernel("test_material_calculateDragForce", 1);
  this->core->addArgument<MaterialCL>("test_material_calculateDragForce", "mat_materials");
  this->core->addArgument<cl_float4>("test_material_calculateDragForce", velocity);
  this->core->addArgument<cl_float4>("test_material_calculateDragForce", rotationVelocity);
  this->core->addArgument<cl_float4>("test_material_calculateDragForce", force);
  this->core->addArgument<cl_float4>("test_material_calculateDragForce", "mat_output");

  this->core->run();
  this->core->clearKernels();

  this->core->readBuffer<cl_float4>("mat_output", output, 0);

  return output[0];
}

void KernelTestHarness::runParticleToParticleWorker(ParticleCL& thisParticle, const ParticleCL& otherParticle,
                                                    const MaterialCL& material, cl_float4& outForce,
                                                    cl_float4& outTorque)
{
  std::vector<ParticleCL> thisVec = {thisParticle};
  std::vector<ParticleCL> otherVec = {otherParticle};
  std::vector<MaterialCL> matVec = {material};
  std::vector<cl_float4> forceVec = {ZERO4};
  std::vector<cl_float4> torqueVec = {ZERO4};

  this->core->writeBuffer<ParticleCL>("pw_this", thisVec, 0);
  this->core->writeBuffer<ParticleCL>("pw_other", otherVec, 0);
  this->core->writeBuffer<MaterialCL>("pw_mat", matVec, 0);
  this->core->writeBuffer<cl_float4>("pw_force", forceVec, 0);
  this->core->writeBuffer<cl_float4>("pw_torque", torqueVec, 0);

  this->core->addKernel("test_particleToParticleWorker", 1);
  this->core->addArgument<ParticleCL>("test_particleToParticleWorker", "pw_this");
  this->core->addArgument<ParticleCL>("test_particleToParticleWorker", "pw_other");
  this->core->addArgument<MaterialCL>("test_particleToParticleWorker", "pw_mat");
  this->core->addArgument<cl_float4>("test_particleToParticleWorker", "pw_force");
  this->core->addArgument<cl_float4>("test_particleToParticleWorker", "pw_torque");

  this->core->run();
  this->core->clearKernels();

  this->core->readBuffer<cl_float4>("pw_force", forceVec, 0);
  this->core->readBuffer<cl_float4>("pw_torque", torqueVec, 0);

  outForce = forceVec[0];
  outTorque = torqueVec[0];
}

void KernelTestHarness::runParticleToFaceWorker(ParticleCL& thisParticle, const FaceCL& otherFace,
                                                const MaterialCL& material, cl_float4& outForce, cl_float4& outTorque)
{
  std::vector<ParticleCL> thisVec = {thisParticle};
  std::vector<FaceCL> faceVec = {otherFace};
  std::vector<MaterialCL> matVec = {material};
  std::vector<cl_float4> forceVec = {ZERO4};
  std::vector<cl_float4> torqueVec = {ZERO4};

  this->core->writeBuffer<ParticleCL>("pfw_this", thisVec, 0);
  this->core->writeBuffer<FaceCL>("pfw_face", faceVec, 0);
  this->core->writeBuffer<MaterialCL>("pfw_mat", matVec, 0);
  this->core->writeBuffer<cl_float4>("pfw_force", forceVec, 0);
  this->core->writeBuffer<cl_float4>("pfw_torque", torqueVec, 0);

  this->core->addKernel("test_particleToFaceWorker", 1);
  this->core->addArgument<ParticleCL>("test_particleToFaceWorker", "pfw_this");
  this->core->addArgument<FaceCL>("test_particleToFaceWorker", "pfw_face");
  this->core->addArgument<MaterialCL>("test_particleToFaceWorker", "pfw_mat");
  this->core->addArgument<cl_float4>("test_particleToFaceWorker", "pfw_force");
  this->core->addArgument<cl_float4>("test_particleToFaceWorker", "pfw_torque");

  this->core->run();
  this->core->clearKernels();

  this->core->readBuffer<cl_float4>("pfw_force", forceVec, 0);
  this->core->readBuffer<cl_float4>("pfw_torque", torqueVec, 0);

  outForce = forceVec[0];
  outTorque = torqueVec[0];
}

void KernelTestHarness::runFaceToParticleWorker(FaceCL& thisFace, const ParticleCL& otherParticle,
                                                const MaterialCL& material, cl_float4& outForce, cl_float4& outTorque)
{
  std::vector<FaceCL> thisVec = {thisFace};
  std::vector<ParticleCL> particleVec = {otherParticle};
  std::vector<MaterialCL> matVec = {material};
  std::vector<cl_float4> forceVec = {ZERO4};
  std::vector<cl_float4> torqueVec = {ZERO4};

  this->core->writeBuffer<FaceCL>("fpw_this", thisVec, 0);
  this->core->writeBuffer<ParticleCL>("fpw_particle", particleVec, 0);
  this->core->writeBuffer<MaterialCL>("fpw_mat", matVec, 0);
  this->core->writeBuffer<cl_float4>("fpw_force", forceVec, 0);
  this->core->writeBuffer<cl_float4>("fpw_torque", torqueVec, 0);

  this->core->addKernel("test_faceToParticleWorker", 1);
  this->core->addArgument<FaceCL>("test_faceToParticleWorker", "fpw_this");
  this->core->addArgument<ParticleCL>("test_faceToParticleWorker", "fpw_particle");
  this->core->addArgument<MaterialCL>("test_faceToParticleWorker", "fpw_mat");
  this->core->addArgument<cl_float4>("test_faceToParticleWorker", "fpw_force");
  this->core->addArgument<cl_float4>("test_faceToParticleWorker", "fpw_torque");

  this->core->run();
  this->core->clearKernels();

  this->core->readBuffer<cl_float4>("fpw_force", forceVec, 0);
  this->core->readBuffer<cl_float4>("fpw_torque", torqueVec, 0);

  outForce = forceVec[0];
  outTorque = torqueVec[0];
}
