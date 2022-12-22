#ifndef PARTICLEWORKER_CL
#define PARTICLEWORKER_CL

#include "../Face.cl"
#include "../Material.cl"
#include "../Neighborhood.cl"
#include "../Particle.cl"

void particleToParticleWorker_run(Particle* thisParticle, ParticleNeighborhood* thisParticleNeighborhood, const Particle* otherParticle, const Material* material, const SimulationExtra* simulationExtra)
{
    if(simulationExtra->useNeighborhood)
        if(!neighborhood_isParticleNeighborToParticle(thisParticleNeighborhood, otherParticle))
            return;

    if(!testBox_particleToParticle(thisParticle, otherParticle, material->distanceThreshold))
        return;

    float4 closestOnThisParticle, closestOnOtherParticle, distanceUnitary;

    particle_getClosestTo(thisParticle, otherParticle, &closestOnThisParticle, &closestOnOtherParticle, &distanceUnitary);

    float4 distance = closestOnOtherParticle - closestOnThisParticle;

    bool internal = particle_isInternal(thisParticle, closestOnOtherParticle);

    if((length(distance) > material->distanceThreshold) && !internal)
        return;

    if(!simulationExtra->useNeighborhood)
        neighborhood_addParticleToParticleNeighborhood(thisParticleNeighborhood, otherParticle);

    float4 velocity = otherParticle->vertex.currentVelocity - thisParticle->vertex.currentVelocity;

    float4 otherR = closestOnOtherParticle - otherParticle->vertex.currentPosition;
    float4 thisR = closestOnThisParticle - thisParticle->vertex.currentPosition;
    float4 rotationVelocity = cross(otherParticle->vertex.currentAngularVelocity, otherR) - cross(thisParticle->vertex.currentAngularVelocity, thisR);

    float minAreaThis  = thisParticle->area  / 2.0;
    float minAreaOther = otherParticle->area / 2.0;

    float  minContactArea = min(minAreaThis, minAreaOther);
    float  originalLength = thisParticle->radius + otherParticle->radius;
    float4 oldForce       = otherParticle->oldForce - thisParticle->oldForce;

    float4 force     = material_calculateForce(material, distance, distanceUnitary, internal, minContactArea, originalLength, oldForce);
    float4 dragForce = material_calculateDragForce(material, velocity, rotationVelocity, force);

    float4 totalForce = - (force + dragForce);

    particle_addCurrentForce(thisParticle, &totalForce, &closestOnThisParticle);
}

bool particleToFaceWorker_run(Particle* thisParticle, ParticleNeighborhood* thisParticleNeighborhood, const Face* otherFace, const Material* material, const SimulationExtra* simulationExtra)
{
    if(simulationExtra->useNeighborhood)
        if(!neighborhood_isParticleNeighborToFace(thisParticleNeighborhood, otherFace))
            return false;

    if(!testBox_particleToFace(thisParticle, otherFace, material->distanceThreshold))
        return false;

    float4 closestOnThisParticle, closestOnOtherFace, distanceUnitary;

    face_getClosestTo(otherFace, thisParticle, &closestOnOtherFace, &closestOnThisParticle, &distanceUnitary);
    distanceUnitary = -distanceUnitary;

    float4 distance = closestOnOtherFace - closestOnThisParticle;

    bool internal = particle_isInternal(thisParticle, closestOnOtherFace);

    if((length(distance) > material->distanceThreshold) && !internal)
        return false;

    if(!simulationExtra->useNeighborhood)
        neighborhood_addFaceToParticleNeighborhood(thisParticleNeighborhood, otherFace);

    float4 velocity = otherFace->currentVelocity - thisParticle->vertex.currentVelocity;

    float4 thisR = closestOnThisParticle - thisParticle->vertex.currentPosition;
    float4 rotationVelocity = - cross(thisParticle->vertex.currentAngularVelocity, thisR);

    float minAreaThis  = thisParticle->area / 2.0;
    float minAreaOther = otherFace->area;

    float  minContactArea = min(minAreaThis, minAreaOther);
    float  originalLength = thisParticle->radius;
    float4 oldForce       = otherFace->oldForce - thisParticle->oldForce;

    float4 force     = material_calculateForce(material, distance, distanceUnitary, internal, minContactArea, originalLength, oldForce);
    float4 dragForce = material_calculateDragForce(material, velocity, rotationVelocity, force);

    float4 totalForce = - (force + dragForce);

    particle_addCurrentForce(thisParticle, &totalForce, &closestOnThisParticle);

    return true;
}

#endif // PARTICLEWORKER_CL
