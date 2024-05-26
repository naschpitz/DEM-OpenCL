#ifndef PARTICLE_CL
#define PARTICLE_CL

#include "../Particle.h.cl"

#include "../Utils.cpp.cl"
#include "../Vector.cpp.cl"
#include "../Vertex.cpp.cl"

void particle_addCurrentForce(Particle* particle, const float4* force, const float4* pointOfForce)
{
    particle->currentForce += (*force);

    float4 r = (*pointOfForce) - particle->vertex.currentPosition;
    particle->currentTorque += cross(r, (*force));
}

void particle_atomicAddCurrentForce(volatile global Particle* particle, const float4* force, const float4* pointOfForce)
{
    atomic_add_float(&(particle->currentForceX), force->x);
    atomic_add_float(&(particle->currentForceY), force->y);
    atomic_add_float(&(particle->currentForceZ), force->z);

    float4 r = (*pointOfForce) - particle->vertex.currentPosition;
    float4 torque = cross(r, (*force));

    atomic_add_float(&(particle->currentTorqueX), torque.x);
    atomic_add_float(&(particle->currentTorqueY), torque.y);
    atomic_add_float(&(particle->currentTorqueZ), torque.z);
}

void particle_getClosestTo(volatile global const Particle* thisParticle, volatile global const Particle* otherParticle, float4 *closestOnThisPaticle, float4 *closestOnOtherParticle)
{
    float4 distance = otherParticle->vertex.currentPosition - thisParticle->vertex.currentPosition;
    float4 distanceUnitary = vector_getUnitary(distance);

    (*closestOnThisPaticle)   = thisParticle->vertex.currentPosition + distanceUnitary * thisParticle->radius;
    (*closestOnOtherParticle) = otherParticle->vertex.currentPosition - distanceUnitary * otherParticle->radius;
}

float4 particle_getCurrentAcceleration(Particle* particle)
{
    return particle->currentForce / particle->mass;
}

float4 particle_getCurrentAngularAcceleration(Particle* particle)
{
    return particle->currentTorque / particle->inertiaMomentum;
}

bool particle_isInternal(volatile global const Particle* particle, float4 vector)
{
    float4 distance = particle->vertex.currentPosition - vector;

    if(length(distance) < particle->radius)
        return true;

    return false;
}

void particle_integrate(Particle* particle, float timeStep)
{
    particle->currentForce = (float4)(particle->currentForce.x, particle->currentForce.y, particle->currentForce.z, 0);
    particle->currentTorque = (float4)(particle->currentTorque.x, particle->currentTorque.y, particle->currentTorque.z, 0);

    //printf("%f %f %f\n", particle->currentForce.x, particle->currentForce.y, particle->currentForce.z);

    particle->vertex.acceleration = particle_getCurrentAcceleration(particle);
    particle->vertex.angularAcceleration = particle_getCurrentAngularAcceleration(particle);

    vertex_integrate(&(particle->vertex), timeStep);

    particle->oldForce = particle->currentForce;
    particle->oldTorque = particle->currentTorque;
}

#endif // PARTICLE_CL
