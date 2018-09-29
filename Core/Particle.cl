#ifndef PARTICLE_CL
#define PARTICLE_CL

#include "../Vector.cl"
#include "../Vertex.cl"

typedef struct
{
    uint index;
    uint materialIndex;

    double radius;
    double density;
    double mass;
    double area;
    double volume;

    double4 currentForce;
    double4 oldForce;

    double4 currentTorque;
    double4 oldTorque;

    Vertex vertex;
} Particle;

void particle_addCurrentForce(Particle* particle, double4 force, double4 pointOfForce)
{
    particle->currentForce += force;

    double4 r = pointOfForce - particle->vertex.currentPosition;
    particle->currentTorque += cross(r, force);
}

void particle_getClosestTo(const Particle* thisParticle, const Particle* otherParticle, double4 *closestOnThisPaticle, double4 *closestOnOtherParticle)
{
    double4 distance = otherParticle->vertex.currentPosition - thisParticle->vertex.currentPosition;
    double4 distanceUnitary = vector_getUnitary(distance);

    (*closestOnThisPaticle)   = thisParticle->vertex.currentPosition + distanceUnitary * thisParticle->radius;
    (*closestOnOtherParticle) = otherParticle->vertex.currentPosition - distanceUnitary * thisParticle->radius;
}

double4 particle_getCurrentAcceleration(const Particle* thisParticle)
{
    return thisParticle->currentForce / thisParticle->mass;
}

bool particle_isInternal(const Particle* thisParticle, const double4 vector)
{
    double4 distance = thisParticle->vertex.currentPosition - vector;

    if(length(distance) < thisParticle->radius)
        return true;

    return false;
}

void particle_integrate(Particle* thisParticle, double timeStep)
{
    double4 currentAcceleration = particle_getCurrentAcceleration(thisParticle);

    thisParticle->vertex.acceleration = currentAcceleration;

    vertex_integrate(&(thisParticle->vertex), timeStep);

    thisParticle->oldForce = thisParticle->currentForce;
    thisParticle->oldTorque = thisParticle->currentTorque;
}

#endif // PARTICLE_CL
