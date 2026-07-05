#ifndef VERTEX_H_CL
#define VERTEX_H_CL

typedef struct
{
    bool fixed;

    float4 originalPosition;
    float4 originalVelocity;
    float4 acceleration;

    float4 currentPosition;
    float4 oldPosition;

    double4 preciseCurrentPosition;
    double4 preciseOldPosition;

    float4 currentVelocity;
    float4 meanVelocity;
    float4 oldVelocity;

    float4 angularAcceleration;

    float4 currentAngularVelocity;
    float4 meanAngularVelocity;
    float4 oldAngularVelocity;
} Vertex;

#endif // VERTEX_H_CL
