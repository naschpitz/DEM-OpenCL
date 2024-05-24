#ifndef PARTICLE_CL
#define PARTICLE_CL

#include "../Particle.h.cl"

#include "../Vector.cpp.cl"
#include "../Vertex.cpp.cl"

void particle_addCurrentForce(Particle* particle, const float4* force, const float4* pointOfForce)
{
    particle->currentForce += (*force);

    float4 r = (*pointOfForce) - particle->vertex.currentPosition;
    particle->currentTorque += cross(r, (*force));
}

void particle_atomicAddCurrentForce(Particle* thisParticle, const float4* force, const float4* pointOfForce)
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

void particle_getClosestTo(const Particle* thisParticle, const Particle* otherParticle, float4 *closestOnThisPaticle, float4 *closestOnOtherParticle)
{
    float4 distance = otherParticle->vertex.currentPosition - thisParticle->vertex.currentPosition;
    float4 distanceUnitary = vector_getUnitary(distance);

    (*closestOnThisPaticle)   = thisParticle->vertex.currentPosition + distanceUnitary * thisParticle->radius;
    (*closestOnOtherParticle) = otherParticle->vertex.currentPosition - distanceUnitary * otherParticle->radius;
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
