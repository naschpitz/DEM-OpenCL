#ifndef PARTICLEWORKER_CL
#define PARTICLEWORKER_CL

#include "../Face.cl"
#include "../Particle.cl"
#include "../Material.cl"

void particleToParticleWorker_run(Particle* thisParticle, const Particle* otherParticle, const Material* material)
{
    if(!testBox_particleToParticle(thisParticle, otherParticle, material->distanceThreshold))
        return;

    float4 closestOnThisParticle, closestOnOtherParticle, distanceUnitary;

    particle_getClosestTo(thisParticle, otherParticle, &closestOnThisParticle, &closestOnOtherParticle, &distanceUnitary);

    float4 distance = closestOnOtherParticle - closestOnThisParticle;

    bool internal = particle_isInternal(thisParticle, closestOnOtherParticle);

    if((length(distance) > material->distanceThreshold) && !internal)
        return;

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

bool particleToFaceWorker_run(Particle* thisParticle, Face* otherFace, const Material* material)
{
    if(!testBox_particleToFace(thisParticle, otherFace, material->distanceThreshold))
        return false;

    float4 closestOnThisParticle, closestOnOtherFace, distanceUnitary;

    face_getClosestTo(otherFace, thisParticle, &closestOnOtherFace, &closestOnThisParticle, &distanceUnitary);
    distanceUnitary = -distanceUnitary;

    float4 distance = closestOnOtherFace - closestOnThisParticle;

    bool internal = particle_isInternal(thisParticle, closestOnOtherFace);

    if((length(distance) > material->distanceThreshold) && !internal)
        return false;

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
