#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include "Face.h"
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

inline FaceCL makeFaceCL(const cl_float4& v0, const cl_float4& v1, const cl_float4& v2)
{
    FaceCL f{};
    f.vertexes[0].currentPosition = v0;
    f.vertexes[1].currentPosition = v1;
    f.vertexes[2].currentPosition = v2;
    return f;
}

#endif // TESTHELPERS_H
