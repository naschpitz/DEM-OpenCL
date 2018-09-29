#ifndef PARTICLEWORKER_CL
#define PARTICLEWORKER_CL

#include "../Particle.cl"
#include "../Material.cl"

void particleWorker_run(Particle* thisParticle, const Particle* otherParticle, const Material* material)
{
    double4 closestOnThisParticle, closestOnOtherParticle;

    particle_getClosestTo(thisParticle, otherParticle, &closestOnThisParticle, &closestOnOtherParticle);

    double4 distance = closestOnOtherParticle - closestOnThisParticle;

    bool internal = particle_isInternal(thisParticle, closestOnOtherParticle);

    if((length(distance) > material->distanceThreshold) && !internal)
        return;

    double4 velocity = otherParticle->vertex.currentVelocity - thisParticle->vertex.currentVelocity;

    double minAreaThis  = thisParticle->area  / 2.0;
    double minAreaOther = otherParticle->area / 2.0;

    double  minContactArea = min(minAreaThis, minAreaOther);
    double  originalLength = thisParticle->radius + otherParticle->radius;
    double4 oldForce       = otherParticle->oldForce - thisParticle->oldForce;

    double4 force     = material_calculateForce(material, distance, internal, minContactArea, originalLength, oldForce);
    double4 dragForce = material_calculateDragForce(material, velocity, distance);

    double4 totalForce = force + dragForce;

    particle_addCurrentForce(thisParticle, -totalForce, closestOnThisParticle);
}

#endif // PARTICLEWORKER_CL
