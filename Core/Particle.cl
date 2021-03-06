#ifndef PARTICLE_CL
#define PARTICLE_CL

#include "../Vector.cl"
#include "../Vertex.cl"

typedef struct
{
    uint index;
    uint materialIndex;

    float radius;
    float density;
    float mass;
    float area;
    float volume;
    float inertiaMomentum;

    float4 currentForce;
    float4 oldForce;

    float4 currentTorque;
    float4 oldTorque;

    Vertex vertex;
} Particle;

void particle_addCurrentForce(Particle* particle, const float4* force, const float4* pointOfForce)
{
    particle->currentForce += (*force);

    float4 r = (*pointOfForce) - particle->vertex.currentPosition;
    particle->currentTorque += cross(r, (*force));
}

void particle_divideCurrentForce(Particle* particle, float divider)
{
    particle->currentForce /= divider;
    particle->currentTorque /= divider;
}

void particle_getClosestTo(const Particle* thisParticle, const Particle* otherParticle, float4 *closestOnThisPaticle, float4 *closestOnOtherParticle, float4 *distanceUnitary)
{
    float4 distance = otherParticle->vertex.currentPosition - thisParticle->vertex.currentPosition;

    (*distanceUnitary)        = vector_getUnitary(distance);
    (*closestOnThisPaticle)   = thisParticle->vertex.currentPosition + (*distanceUnitary) * thisParticle->radius;
    (*closestOnOtherParticle) = otherParticle->vertex.currentPosition - (*distanceUnitary) * otherParticle->radius;
}

float4 particle_getCurrentAcceleration(const Particle* particle)
{
    return particle->currentForce / particle->mass;
}

float4 particle_getCurrentAngularAcceleration(const Particle* particle)
{
    return particle->currentTorque / particle->inertiaMomentum;
}

bool particle_isInternal(const Particle* particle, const float4 vector)
{
    float4 distance = particle->vertex.currentPosition - vector;

    if(length(distance) < particle->radius)
        return true;

    return false;
}

void particle_integrate(Particle* particle, float timeStep)
{
    particle->vertex.acceleration = particle_getCurrentAcceleration(particle);
    particle->vertex.angularAcceleration = particle_getCurrentAngularAcceleration(particle);

    vertex_integrate(&(particle->vertex), timeStep);

    particle->oldForce = particle->currentForce;
    particle->oldTorque = particle->currentTorque;
}

#endif // PARTICLE_CL
