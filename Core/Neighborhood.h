#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 300
#endif

#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include <CL/opencl.hpp>

#define MAX_PARTICLES_TO_PARTICLES 1024
#define MAX_FACES_TO_PARTICLES 64

#define MAX_PARTICLES_TO_FACES 1024

typedef struct
{
    cl_uint numParticles;
    cl_uint numFaces;

    cl_uint particles[MAX_PARTICLES_TO_PARTICLES];
    cl_uint faces[MAX_FACES_TO_PARTICLES];
} ParticleNeighborhoodCL;

typedef struct
{
    cl_uint numParticles;

    cl_uint particles[MAX_PARTICLES_TO_FACES];
} FaceNeighborhoodCL;

#endif // NEIGHBORHOOD_H
