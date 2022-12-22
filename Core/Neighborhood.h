#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 210
#endif

#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include <CL/cl2.hpp>

#define MAX_PARTICLES_TO_PARTICLES 128
#define MAX_FACES_TO_PARTICLES 16

#define MAX_PARTICLES_TO_FACES 512

#include <QVector>

#include "Particle.h"
#include "Face.h"

typedef struct
{
    cl_uint numParticles;
    cl_uint numFaces;

    cl_uint particles[MAX_PARTICLES_TO_PARTICLES];
    cl_int faces[MAX_FACES_TO_PARTICLES];
} ParticleNeighborhoodCL;

typedef struct
{
    cl_uint numParticles;

    cl_uint particles[MAX_PARTICLES_TO_FACES];
} FaceNeighborhoodCL;

class Neighborhood
{
    public:
        static QVector<ParticleNeighborhoodCL> getParticlesCL(uint particlesCount);
        static QVector<FaceNeighborhoodCL>     getFacesCL(uint facesCount);
};

#endif // NEIGHBORHOOD_H
