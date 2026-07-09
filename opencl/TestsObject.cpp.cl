#ifndef TESTSOBJECT_CPP_CL
#define TESTSOBJECT_CPP_CL

#include "../opencl/Face.cpp.cl"

kernel void test_object_to_particle(global Face* faces, const uint numFaces, global Particle* particles,
                                    global float4* outputFace, global float4* outputParticle)
{
  size_t i = get_global_id(0);

  const Particle* particle = &particles[i];

  float4 bestFace = (float4)0;
  float4 bestParticle = (float4)0;
  float bestDist2 = FLT_MAX;

  for (uint f = 0; f < numFaces; f++) {
    float4 closestFace;
    float4 closestParticle;

    face_getClosestTo(&faces[f], particle, &closestFace, &closestParticle);

    float4 delta = particle->vertex.currentPosition - closestFace;
    float dist2 = dot(delta, delta);

    if (dist2 < bestDist2) {
      bestDist2 = dist2;
      bestFace = closestFace;
      bestParticle = closestParticle;
    }
  }

  outputFace[i] = bestFace;
  outputParticle[i] = bestParticle;
}

#endif // TESTSOBJECT_CPP_CL
