#ifndef VERTEX_CL
#define VERTEX_CL

typedef struct
{
    bool fixed;

    double4 originalPosition;
    double4 originalVelocity;
    double4 acceleration;

    double4 currentPosition;
    double4 oldPosition;

    double4 currentVelocity;
    double4 oldVelocity;
} Vertex;


void vertex_integrate(Vertex* vertex, double timeStep)
{
    double4 tempPosition;

    // Stores the current position.
    tempPosition = vertex->currentPosition;

    if(vertex->fixed)
        vertex->currentPosition += vertex->currentVelocity * timeStep;

    else
    {
        // Calculates the new position based on Verlet integration method.
        vertex->currentPosition = 2 * vertex->currentPosition - vertex->oldPosition + vertex->acceleration * timeStep * timeStep;

        // Calculates the new speed.
        vertex->currentVelocity = (vertex->currentPosition - tempPosition) / timeStep;
    }

    // Recovers the position stored before and assigns it to the vertice's past position field.
    vertex->oldPosition = tempPosition;

    // Resets the acceleration.
    vertex->acceleration = 0;
}

#endif // VERTEX_CL
