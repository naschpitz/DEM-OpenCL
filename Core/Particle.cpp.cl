#ifndef PARTICLE_CL
#define PARTICLE_CL

#include "../Particle.h.cl"
#include "../Neighborhood.h.cl"

#include "../Vector.cpp.cl"
#include "../Vertex.cpp.cl"

void particle_addCurrentForceToParticle(Particle* thisParticle, const Particle* otherParticle, const float4* force, const float4* pointOfForceThisParticle, const float4* pointOfForceOtherParticle, global ForceTorque* particlesToParticlesNeighborhood, global uint* particlesToParticlesNeighborhoodNum)
{
    uint thisParticleIndex = thisParticle->index;
    uint otherParticleIndex = otherParticle->index;

    thisParticle->currentForce += (*force);

    float4 thisR = (*pointOfForceThisParticle) - thisParticle->vertex.currentPosition;
    float4 thisTorque = cross(thisR, (*force));

    thisParticle->currentTorque += thisTorque;

    float4 otherForce = -(*force);

    float4 otherR = (*pointOfForceOtherParticle) - otherParticle->vertex.currentPosition;
    float4 otherTorque = cross(otherR, (*force));

    // Find thisParticle in otherParticle's neighborhood and store the reaction force
    uint numParticles = particlesToParticlesNeighborhoodNum[otherParticleIndex];
    for(ulong i = 0; i < numParticles; i++) {
        uint neighborIndex = otherParticleIndex * MAX_PARTICLES_TO_PARTICLES + i;

        if(particlesToParticlesNeighborhood[neighborIndex].index == thisParticleIndex) {
            particlesToParticlesNeighborhood[neighborIndex].currentForce = otherForce;
            particlesToParticlesNeighborhood[neighborIndex].currentTorque = otherTorque;
            break;
        }
    }
}

void particle_addCurrentForceToFace(Particle* thisParticle, const Face* otherFace, const float4* force, const float4* pointOfForceThisParticle, const float4* pointOfForceOtherFace, global ForceTorque* particlesToFacesNeighborhood, global uint* particlesToFacesNeighborhoodNum)
{
    uint thisParticleIndex = thisParticle->index;
    uint otherFaceIndex = otherFace->index;

    thisParticle->currentForce += (*force);

    float4 thisR = (*pointOfForceThisParticle) - thisParticle->vertex.currentPosition;
    float4 thisTorque = cross(thisR, (*force));

    thisParticle->currentTorque += thisTorque;

    float4 otherForce = -(*force);

    float4 otherR = (*pointOfForceOtherFace) - otherFace->currentPosition;
    float4 otherTorque = cross(otherR, (*force));

    // Find thisParticle in otherFace's neighborhood and store the reaction force
    uint numParticles = particlesToFacesNeighborhoodNum[otherFaceIndex];
    for(ulong i = 0; i < numParticles; i++) {
        uint neighborIndex = otherFaceIndex * MAX_PARTICLES_TO_FACES + i;
        
        if(particlesToFacesNeighborhood[neighborIndex].index == thisParticleIndex) {
            particlesToFacesNeighborhood[neighborIndex].currentForce = otherForce;
            particlesToFacesNeighborhood[neighborIndex].currentTorque = otherTorque;
            break;
        }
    }
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

void particle_sumForces(Particle* particle, global ForceTorque* particlesToParticlesNeighborhood, global uint* particlesToParticlesNeighborhoodNum)
{
    uint thisParticleIndex = particle->index;

    uint numParticles = particlesToParticlesNeighborhoodNum[thisParticleIndex];

    for(ulong i = 0; i < numParticles; i++) {
        uint neighborIndex = thisParticleIndex * MAX_PARTICLES_TO_PARTICLES + i;
        particle->currentForce += particlesToParticlesNeighborhood[neighborIndex].currentForce;
        particle->currentTorque += particlesToParticlesNeighborhood[neighborIndex].currentTorque;
    }
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
