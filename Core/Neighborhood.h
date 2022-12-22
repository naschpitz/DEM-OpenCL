#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 210
#endif

#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include <CL/cl2.hpp>

#define MAX_PARTICLES 131072
#define MAX_FACES 65536

#include <QVector>

#include "Particle.h"
#include "Face.h"

typedef struct
{
    // cl_bool has actually the size of an unsinged int, 32 bits.
    cl_bool particles[MAX_PARTICLES/32];
} ParticleNeighborhoodCL;

typedef struct
{
    // cl_bool has actually the size of an unsinged int, 32 bits.
    cl_bool faces[MAX_FACES/32];
} FaceNeighborhoodCL;

class Neighborhood
{
    public:
        static QVector<ParticleNeighborhoodCL> getParticlesCL(uint particlesCount);
        static QVector<FaceNeighborhoodCL>     getFacesCL(uint facesCount);
};

#endif // NEIGHBORHOOD_H
