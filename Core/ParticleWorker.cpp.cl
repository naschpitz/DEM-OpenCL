#ifndef PARTICLEWORKER_CPP_CL
#define PARTICLEWORKER_CPP_CL

#include "../Face.cpp.cl"
#include "../Material.cpp.cl"
#include "../Particle.cpp.cl"

void particleToParticleWorker_run(Particle* thisParticle, Particle* otherParticle, const Material* material)
{
    float4 closestOnThisParticle, closestOnOtherParticle;

    particle_getClosestTo(thisParticle, otherParticle, &closestOnThisParticle, &closestOnOtherParticle);

    float4 distance = closestOnOtherParticle - closestOnThisParticle;
    float4 distanceUnitary = vector_getUnitary(distance);

    bool internal = particle_isInternal(thisParticle, closestOnOtherParticle);

    if((length(distance) > material->distanceThreshold) && !internal)
        return;

    float4 velocity = otherParticle->vertex.currentVelocity - thisParticle->vertex.currentVelocity;

    float4 otherR = closestOnOtherParticle - otherParticle->vertex.currentPosition;
    float4 thisR = closestOnThisParticle - thisParticle->vertex.currentPosition;
    float4 rotationVelocity = cross(otherParticle->vertex.currentAngularVelocity, otherR) - cross(thisParticle->vertex.currentAngularVelocity, thisR);

    float minAreaThis  = thisParticle->area  / 4.0;
    float minAreaOther = otherParticle->area / 4.0;

    float  minContactArea = min(minAreaThis, minAreaOther);
    float  originalLength = thisParticle->radius + otherParticle->radius;
    float4 oldForce       = otherParticle->oldForce - thisParticle->oldForce;

    float4 force     = material_calculateForce(material, distance, distanceUnitary, internal, minContactArea, originalLength, oldForce);
    float4 dragForce = material_calculateDragForce(material, velocity, rotationVelocity, force);

    float4 totalForce = - (force + dragForce);

    particle_atomicAddCurrentForce(thisParticle, otherParticle, &totalForce, &closestOnThisParticle);
}

bool particleToFaceWorker_run(Particle* thisParticle, Face* otherFace, const Material* material)
{
    float4 closestOnThisParticle, closestOnOtherFace;

    face_getClosestTo(otherFace, thisParticle, &closestOnOtherFace, &closestOnThisParticle);

    float4 distance = closestOnOtherFace - closestOnThisParticle;
    float4 distanceUnitary = vector_getUnitary(distance);

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

#endif // PARTICLEWORKER_CPP_CL
