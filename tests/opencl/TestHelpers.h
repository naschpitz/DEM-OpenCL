#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include "Face.h"
#include "Particle.h"
#include "Vertex.h"

#include <vector>

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

inline std::vector<FaceCL> makeUnitCubeCL()
{
    cl_float4 c000 = {0.0f, 0.0f, 0.0f, 0.0f};
    cl_float4 c100 = {1.0f, 0.0f, 0.0f, 0.0f};
    cl_float4 c010 = {0.0f, 1.0f, 0.0f, 0.0f};
    cl_float4 c110 = {1.0f, 1.0f, 0.0f, 0.0f};
    cl_float4 c001 = {0.0f, 0.0f, 1.0f, 0.0f};
    cl_float4 c101 = {1.0f, 0.0f, 1.0f, 0.0f};
    cl_float4 c011 = {0.0f, 1.0f, 1.0f, 0.0f};
    cl_float4 c111 = {1.0f, 1.0f, 1.0f, 0.0f};

    return {
        makeFaceCL(c001, c101, c111),
        makeFaceCL(c001, c111, c011),
        makeFaceCL(c000, c110, c100),
        makeFaceCL(c000, c010, c110),
        makeFaceCL(c100, c110, c111),
        makeFaceCL(c100, c111, c101),
        makeFaceCL(c000, c011, c010),
        makeFaceCL(c000, c001, c011),
        makeFaceCL(c010, c110, c111),
        makeFaceCL(c010, c111, c011),
        makeFaceCL(c000, c101, c100),
        makeFaceCL(c000, c001, c101)
    };
}

#endif // TESTHELPERS_H
