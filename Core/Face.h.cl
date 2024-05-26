#ifndef FACE_H_CL
#define FACE_H_CL

#include "../Vertex.h.cl"

typedef struct
{
    uint index;
    uint materialIndex;

    float area;
    float mass;

    float4 currentPosition;
    float4 currentVelocity;

    float currentForceX, currentForceY, currentForceZ;
    float4 currentForce;
    float4 oldForce;

    float currentTorqueX, currentTorqueY, currentTorqueZ;
    float4 currentTorque;
    float4 oldTorque;

    Vertex vertexes[3];
} Face;

#endif // FACE_H_CL
