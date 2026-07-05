#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include "Particle.h"
#include "Vertex.h"

inline VertexCL makeVertexCL(float x, float y, float z)
{
    VertexCL v{};
    v.currentPosition = {x, y, z, 0.0f};
    return v;
}

inline ParticleCL makeParticleCL(float x, float y, float z, float radius)
{
    ParticleCL p{};
    p.vertex.currentPosition = {x, y, z, 0.0f};
    p.radius = radius;
    return p;
}

#endif // TESTHELPERS_H
