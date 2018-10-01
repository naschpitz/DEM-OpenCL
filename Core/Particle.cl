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

void particle_addCurrentForce(Particle* particle, const double4* force, const double4* pointOfForce)
{
    particle->currentForce += (*force);

    double4 r = (*pointOfForce) - particle->vertex.currentPosition;
    particle->currentTorque += cross(r, (*force));
}

void particle_getClosestTo(const Particle* thisParticle, const Particle* otherParticle, double4 *closestOnThisPaticle, double4 *closestOnOtherParticle)
{
    double4 distance = otherParticle->vertex.currentPosition - thisParticle->vertex.currentPosition;
    double4 distanceUnitary = vector_getUnitary(distance);

    (*closestOnThisPaticle)   = thisParticle->vertex.currentPosition + distanceUnitary * thisParticle->radius;
    (*closestOnOtherParticle) = otherParticle->vertex.currentPosition - distanceUnitary * otherParticle->radius;
}

double4 particle_getCurrentAcceleration(const Particle* particle)
{
    return particle->currentForce / particle->mass;
}

bool particle_isInternal(const Particle* particle, const double4 vector)
{
    double4 distance = particle->vertex.currentPosition - vector;

    if(length(distance) < particle->radius)
        return true;

    return false;
}

void particle_integrate(Particle* particle, double timeStep)
{
    double4 currentAcceleration = particle_getCurrentAcceleration(particle);

    particle->vertex.acceleration = currentAcceleration;

    vertex_integrate(&(particle->vertex), timeStep);

    particle->oldForce = particle->currentForce;
    particle->oldTorque = particle->currentTorque;
}

#endif // PARTICLE_CL
