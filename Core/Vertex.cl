#ifndef VERTEX_CL
#define VERTEX_CL

typedef struct
{
    bool fixed;

    float4 originalPosition;
    float4 originalVelocity;
    float4 acceleration;

    float4 currentPosition;
    float4 oldPosition;

    float4 currentVelocity;
    float4 oldVelocity;

    float4 angularAcceleration;

    float4 currentAngularVelocity;
    float4 oldAngularVelocity;
} Vertex;


void vertex_integrate(Vertex* vertex, float timeStep)
{
    float4 tempPosition;

    // Stores the current position.
    tempPosition = vertex->currentPosition;

    if(vertex->fixed)
        vertex->currentPosition += vertex->currentVelocity * timeStep;

    else
    {
        // Calculates the new position based on Verlet integration method.
        vertex->currentPosition = 2 * vertex->currentPosition - vertex->oldPosition + vertex->acceleration * timeStep * timeStep;

        // Calculates the new speed.
        vertex->oldVelocity = vertex->currentVelocity;
        vertex->currentVelocity = (vertex->currentPosition - tempPosition) / timeStep;

        vertex->oldAngularVelocity = vertex->currentAngularVelocity;
        vertex->currentAngularVelocity += vertex->angularAcceleration * timeStep;
    }

    // Recovers the position stored before and assigns it to the vertice's past position field.
    vertex->oldPosition = tempPosition;

    // Resets the acceleration.
    vertex->acceleration = 0;
    vertex->angularAcceleration = 0;
}

#endif // VERTEX_CL
