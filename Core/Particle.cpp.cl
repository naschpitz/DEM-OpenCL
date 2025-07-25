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

void particle_divideCurrentForce(Particle* particle, float divider)
{
    if(divider == 0)
        return;

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

void particle_integrate(Particle* particle, const Simulation* simulation)
{
    uint stepsPerFrame = simulation->frameTime / simulation->timeStep;

    particle->meanForce  += (particle->currentForce / stepsPerFrame);
    particle->meanTorque += (particle->currentTorque / stepsPerFrame);

    particle->vertex.acceleration = particle_getCurrentAcceleration(particle);
    particle->vertex.angularAcceleration = particle_getCurrentAngularAcceleration(particle);

    vertex_integrate(&(particle->vertex), simulation);

    particle->oldForce = particle->currentForce;
    particle->oldTorque = particle->currentTorque;
}

void particle_reset(Particle* particle, const Simulation* simulation)
{
    particle->currentForce = (0, 0, 0, 0);
    particle->oldForce     = (0, 0, 0, 0);

    particle->currentTorque = (0, 0, 0, 0);
    particle->oldTorque     = (0, 0, 0, 0);

    uint stepsPerFrame = simulation->frameTime / simulation->timeStep;

    if (simulation->currentStep % stepsPerFrame == 0) {
        particle->meanForce  = (0, 0, 0, 0);
        particle->meanTorque = (0, 0, 0, 0);

        vertex_reset(&(particle->vertex));
    }  
}

#endif // PARTICLE_CL
