#ifndef TESTSFACEWORKER_CPP_CL
#define TESTSFACEWORKER_CPP_CL

#include "../opencl/FaceWorker.cpp.cl"

kernel void test_faceToParticleWorker(global Face* thisFace, global const Particle* otherParticle,
                                      global const Material* material, global float4* outputForce,
                                      global float4* outputTorque)
{
  faceToParticleWorker_run(thisFace, otherParticle, material);

  outputForce[0] = thisFace->currentForce;
  outputTorque[0] = thisFace->currentTorque;
}

#endif // TESTSFACEWORKER_CPP_CL
