#ifndef REDUCEDVERTEX_CL
#define REDUCEDVERTEX_CL

#include "../Vertex.cl"

typedef struct
{
    bool fixed;

    double4 currentPosition;
    double4 currentVelocity;
} ReducedVertex;

ReducedVertex reducedVertex_fromVertex(const Vertex* vertex)
{
    ReducedVertex reducedVertex;

    reducedVertex.fixed = vertex->fixed;

    reducedVertex.currentPosition = vertex->currentPosition;
    reducedVertex.currentVelocity = vertex->currentVelocity;

    return reducedVertex;
}

Vertex reducedVertex_toVertex(const ReducedVertex* reducedVertex)
{
    Vertex vertex;

    vertex.fixed = reducedVertex->fixed;

    vertex.currentPosition = reducedVertex->currentPosition;
    vertex.currentVelocity = reducedVertex->currentVelocity;

    return vertex;
}

#endif // REDUCEDVERTEX_CL
