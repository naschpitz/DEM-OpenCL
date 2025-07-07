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
#define MAX_FACES_TO_PARTICLES 512

#define MAX_PARTICLES_TO_FACES 2048
#define MAX_FACES_TO_FACES 256

typedef struct {
    cl_uint index;
    cl_float4 currentForce;
    cl_float4 currentTorque;
} ForceTorqueCL;

// ParticleNeighborhoodCL and FaceNeighborhoodCL structs removed
// Now using separate buffers for better memory management

#endif // NEIGHBORHOOD_H
