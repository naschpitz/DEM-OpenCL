#ifndef TESTSPARTICLEWORKER_CPP_CL
#define TESTSPARTICLEWORKER_CPP_CL

#include "../opencl/ParticleWorker.cpp.cl"

kernel void test_particleToParticleWorker(
    global Particle* thisParticle,
    global const Particle* otherParticle,
    global const Material* material,
    global float4* outputForce,
    global float4* outputTorque)
{
    particleToParticleWorker_run(thisParticle, otherParticle, material);

    outputForce[0]  = thisParticle->currentForce;
    outputTorque[0] = thisParticle->currentTorque;
}

kernel void test_particleToFaceWorker(
    global Particle* thisParticle,
    global Face* otherFace,
    global const Material* material,
    global float4* outputForce,
    global float4* outputTorque)
{
    particleToFaceWorker_run(thisParticle, otherFace, material);

    outputForce[0]  = thisParticle->currentForce;
    outputTorque[0] = thisParticle->currentTorque;
}

#endif // TESTSPARTICLEWORKER_CPP_CL
