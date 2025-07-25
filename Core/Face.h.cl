#ifndef FACE_H_CL
#define FACE_H_CL

#include "../Neighborhood.h.cl"
#include "../Vertex.h.cl"

typedef struct
{
    uint index;
    uint materialIndex;

    float area;
    float mass;

    float4 currentPosition;

    float4 currentVelocity;
    float4 meanVelocity;

    float4 currentForce;
    float4 meanForce;
    float4 oldForce;

    float4 currentTorque;
    float4 meanTorque;
    float4 oldTorque;

    Vertex vertexes[3];

    FaceNeighborhood neighborhood;
} Face;

#endif // FACE_H_CL
