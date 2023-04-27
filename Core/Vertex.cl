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

    double4 preciseCurrentPosition;
    double4 preciseOldPosition;

    float4 currentVelocity;
    float4 oldVelocity;

    float4 angularAcceleration;

    float4 currentAngularVelocity;
    float4 oldAngularVelocity;
} Vertex;


void vertex_integrate(Vertex* vertex, float timeStep)
{
    // Stores the current position.
    float4 tempPosition = vertex->currentPosition;
    double4 preciseTempPosition = vertex->preciseCurrentPosition;

    if(vertex->fixed) {
        vertex->currentPosition += vertex->currentVelocity * timeStep;
        vertex->preciseCurrentPosition = convert_double4(vertex->currentPosition);
    }

    else
    {
        // Calculates the new position based on Verlet integration method.
        vertex->preciseCurrentPosition = 2 * vertex->preciseCurrentPosition - vertex->preciseOldPosition + convert_double4(vertex->acceleration) * timeStep * timeStep;
        vertex->currentPosition = convert_float4(vertex->preciseCurrentPosition);

        // Calculates the new speed.
        vertex->oldVelocity = vertex->currentVelocity;
        vertex->currentVelocity = (vertex->currentPosition - tempPosition) / timeStep;

        vertex->oldAngularVelocity = vertex->currentAngularVelocity;
        vertex->currentAngularVelocity += vertex->angularAcceleration * timeStep;
    }

    // Recovers the position stored before and assigns it to the vertice's past position field.
    vertex->oldPosition = tempPosition;
    vertex->preciseOldPosition = preciseTempPosition;

    // Resets the acceleration.
    vertex->acceleration = 0;
    vertex->angularAcceleration = 0;
}

#endif // VERTEX_CL
