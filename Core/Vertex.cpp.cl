#ifndef VERTEX_CPP_CL
#define VERTEX_CPP_CL

#include "../Vertex.h.cl"

void vertex_integrate(Vertex* vertex, const Simulation* simulation)
{
    float timeStep = simulation->timeStep;
    uint stepsPerFrame = simulation->frameTime / timeStep;

    // Stores the current position.
    float4 tempPosition = vertex->currentPosition;
    double4 preciseTempPosition = vertex->preciseCurrentPosition;

    if(vertex->fixed) {
        vertex->preciseCurrentPosition += convert_double4(vertex->currentVelocity) * timeStep;
        vertex->currentPosition = convert_float4(vertex->preciseCurrentPosition);
    }

    else
    {
        // Calculates the new position based on Verlet integration method.
        vertex->preciseCurrentPosition = 2 * vertex->preciseCurrentPosition - vertex->preciseOldPosition + convert_double4(vertex->acceleration) * timeStep * timeStep;
        vertex->currentPosition = convert_float4(vertex->preciseCurrentPosition);

        // Calculates the new speed.
        vertex->oldVelocity = vertex->currentVelocity;
        vertex->currentVelocity = (vertex->currentPosition - tempPosition) / timeStep;
        vertex->meanVelocity += vertex->currentVelocity / stepsPerFrame;

        vertex->oldAngularVelocity = vertex->currentAngularVelocity;
        vertex->currentAngularVelocity += vertex->angularAcceleration * timeStep;
        vertex->meanAngularVelocity += vertex->currentAngularVelocity / stepsPerFrame;
    }

    // Recovers the position stored before and assigns it to the vertice's past position field.
    vertex->oldPosition = tempPosition;
    vertex->preciseOldPosition = preciseTempPosition;

    // Resets the acceleration.
    vertex->acceleration = 0;
    vertex->angularAcceleration = 0;
}

void vertex_reset(Vertex* vertex)
{
    vertex->meanVelocity        = (0, 0, 0, 0);
    vertex->meanAngularVelocity = (0, 0, 0, 0);
}

#endif // VERTEX_CPP_CL
