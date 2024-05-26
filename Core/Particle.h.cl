#ifndef PARTICLE_H_CL
#define PARTICLE_H_CL

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

    float currentForceX, currentForceY, currentForceZ;
    float4 currentForce;
    float4 oldForce;

    float currentTorqueX, currentTorqueY, currentTorqueZ;
    float4 currentTorque;
    float4 oldTorque;

    Vertex vertex;
} Particle;

#endif // PARTICLE_H_CL
