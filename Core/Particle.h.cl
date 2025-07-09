#ifndef PARTICLE_H_CL
#define PARTICLE_H_CL

#include "../Neighborhood.h.cl"
#include "../Vertex.h.cl"

typedef struct
{
    uint index;
    uint materialIndex;

    float radius;
    float density;
    float mass;
    float area;
    float volume;
    float inertiaMomentum;

    float4 currentForce;
    float4 meanForce;
    float4 oldForce;

    float4 currentTorque;
    float4 meanTorque;
    float4 oldTorque;

    Vertex vertex;

    ParticleNeighborhood neighborhood;
} Particle;

#endif // PARTICLE_H_CL
