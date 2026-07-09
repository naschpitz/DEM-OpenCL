#ifndef TESTSFACE_CPP_CL
#define TESTSFACE_CPP_CL

#include "../opencl/Face.cpp.cl"

kernel void test_face_getClosestToParticle(global Face* faces, global Particle* particles, global float4* outputFace,
                                           global float4* outputParticle)
{
  size_t i = get_global_id(0);

  const Face* face = &faces[i];
  const Particle* particle = &particles[i];

  float4 closestFace = (float4)0;
  float4 closestParticle = (float4)0;

  face_getClosestToParticle(face, particle, &closestFace, &closestParticle);

  outputFace[i] = closestFace;
  outputParticle[i] = closestParticle;
}

#endif // TESTSFACE_CPP_CL
