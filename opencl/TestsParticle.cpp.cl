#ifndef TESTSPARTICLE_CPP_CL
#define TESTSPARTICLE_CPP_CL

#include "../opencl/Particle.cpp.cl"

kernel void test_particle_getClosestTo(global Particle* particles, global float4* outputThis,
                                       global float4* outputOther)
{
  size_t i = get_global_id(0);

  const Particle* thisP = &particles[i * 2];
  const Particle* otherP = &particles[i * 2 + 1];

  float4 closestThis = (float4)0;
  float4 closestOther = (float4)0;

  particle_getClosestTo(thisP, otherP, &closestThis, &closestOther);

  outputThis[i] = closestThis;
  outputOther[i] = closestOther;
}

kernel void test_particle_isInternal(global Particle* particles, global float4* queryPoints, global uint* output)
{
  size_t i = get_global_id(0);

  const Particle* p = &particles[i];

  output[i] = particle_isInternal(p, queryPoints[i]) ? 1u : 0u;
}

#endif // TESTSPARTICLE_CPP_CL
