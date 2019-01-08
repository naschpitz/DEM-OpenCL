#ifndef FACEWORKER_CL
#define FACEWORKER_CL

#include "../Face.cl"
#include "../Particle.cl"
#include "../Material.cl"
#include "../TestBox.cl"

void faceToParticleWorker_run(Face* thisFace, Particle* otherParticle, const Material* material)
{
    if (!testBox_particleToFace(otherParticle, thisFace, material->distanceThreshold))
        return;

    double4 closestOnThisFace, closestOnOtherParticle, distanceUnitary;

    face_getClosestTo(thisFace, otherParticle, &closestOnThisFace, &closestOnOtherParticle, &distanceUnitary);

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

    double4 force     = material_calculateForce(material, distance, distanceUnitary, internal, minContactArea, originalLength, oldForce);
    double4 dragForce = material_calculateDragForce(material, velocity, distance);

    double4 totalForce = -(force + dragForce);

    face_addCurrentForce(thisFace, &totalForce, &closestOnThisFace);
}

#endif // FACEWORKER_CL
