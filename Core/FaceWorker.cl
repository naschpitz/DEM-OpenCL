#ifndef FACEWORKER_CL
#define FACEWORKER_CL

#include "../Face.cl"
#include "../Particle.cl"
#include "../Material.cl"

void faceToParticleWorker_run(Face* thisFace, Particle* otherParticle, const Material* material)
{
    face_calculateCurrentPosition(thisFace);
    face_calculateCurrentVelocity(thisFace);

    double4 closestOnThisFace, closestOnOtherParticle;

    face_getClosestTo(thisFace, otherParticle, &closestOnThisFace, &closestOnOtherParticle);

    double4 distance = closestOnOtherParticle - closestOnThisFace;

    bool internal = particle_isInternal(otherParticle, closestOnThisFace);

    if((length(distance) > material->distanceThreshold) && !internal)
        return;

    double4 velocity = otherParticle->vertex.currentVelocity - thisFace->currentVelocity;

    double minAreaThis  = thisFace->area;
    double minAreaOther = otherParticle->area / 2.0;

    double  minContactArea = min(minAreaThis, minAreaOther);
    double  originalLength = otherParticle->radius;
    double4 oldForce       = otherParticle->oldForce - thisFace->oldForce;

    double4 force     = material_calculateForce(material, distance, internal, minContactArea, originalLength, oldForce);
    double4 dragForce = material_calculateDragForce(material, velocity, distance);

    double4 totalForce = -(force + dragForce);

    face_addCurrentForce(thisFace, &totalForce, &closestOnThisFace);
}

#endif // FACEWORKER_CL
