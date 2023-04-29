#ifndef EDGE_CPP_CL
#define EDGE_CPP_CL

#include "../Edge.h.cl"

float4 edge_getDistance(const Edge* edge)
{
    return edge->v2.currentPosition - edge->v1.currentPosition;
}

void edge_getClosestTo(const Edge* thisEdge, const float4* otherVector, float4* closestOnThisEdge)
{
    const float4 p1 = thisEdge->v1.currentPosition;
    const float4 p2 = thisEdge->v2.currentPosition;

    const float4 q = (*otherVector);

    float4 a = q - p1;
    float4 b = edge_getDistance(thisEdge);

    float u = dot(a, b) / dot(b, b);

    if(u <= 0)
        (*closestOnThisEdge) = p1;

    else if(u >= 1)
        (*closestOnThisEdge) = p2;

    else
        (*closestOnThisEdge) = p1 + u * b;
}

#endif // EDGE_CPP_CL
