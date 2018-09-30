#ifndef EDGE_CL
#define EDGE_CL

#include "../Vertex.cl"

typedef struct
{
    Vertex v1, v2;
} Edge;

double4 edge_getDistance(const Edge* edge)
{
    return edge->v2.currentPosition - edge->v1.currentPosition;
}

void edge_getClosestTo(const Edge* thisEdge, const double4* otherVector, double4* closestOnThisEdge)
{
    const double4 p1 = thisEdge->v1.currentPosition;
    const double4 p2 = thisEdge->v2.currentPosition;

    const double4 q = (*otherVector);

    double4 a = q - p1;
    double4 b = edge_getDistance(thisEdge);

    double u = dot(a, b) / dot(b, b);

    if(u <= 0)
        (*closestOnThisEdge) = p1;

    else if(u >= 1)
        (*closestOnThisEdge) = p2;

    else
        (*closestOnThisEdge) = p1 + u * b;
}

#endif // EDGE_CL
