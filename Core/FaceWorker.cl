#ifndef FACEWORKER_CL
#define FACEWORKER_CL

#include "../Face.cl"
#include "../Material.cl"
#include "../Neighborhood.cl"
#include "../Particle.cl"
#include "../SimulationExtra.cl"
#include "../TestBox.cl"

void faceToParticleWorker_run(Face* thisFace, FaceNeighborhood* thisFaceNeighborhood, const Particle* otherParticle, const Material* material, const SimulationExtra* simulationExtra)
{
    if(simulationExtra->useNeighborhood)
        if(!neighborhood_isFaceNeighborToParticle(thisFaceNeighborhood, otherParticle))
            return;

    if(!testBox_particleToFace(otherParticle, thisFace, material->distanceThreshold))
        return;

    float4 closestOnThisFace, closestOnOtherParticle, distanceUnitary;

    face_getClosestTo(thisFace, otherParticle, &closestOnThisFace, &closestOnOtherParticle, &distanceUnitary);

    float4 distance = closestOnOtherParticle - closestOnThisFace;

    bool internal = particle_isInternal(otherParticle, closestOnThisFace);

    if((length(distance) > material->distanceThreshold) && !internal)
        return;

    if(!simulationExtra->useNeighborhood)
        neighborhood_addParticleToFaceNeighborhood(thisFaceNeighborhood, otherParticle);

    float4 velocity = otherParticle->vertex.currentVelocity - thisFace->currentVelocity;

    float4 otherR = closestOnOtherParticle - otherParticle->vertex.currentPosition;
    float4 rotationVelocity = cross(otherParticle->vertex.currentAngularVelocity, otherR);

    float minAreaThis  = thisFace->area;
    float minAreaOther = otherParticle->area / 2.0;

    float  minContactArea = min(minAreaThis, minAreaOther);
    float  originalLength = otherParticle->radius;
    float4 oldForce       = otherParticle->oldForce - thisFace->oldForce;

    float4 force     = material_calculateForce(material, distance, distanceUnitary, internal, minContactArea, originalLength, oldForce);
    float4 dragForce = material_calculateDragForce(material, velocity, rotationVelocity, force);

    float4 totalForce = -(force + dragForce);

    face_addCurrentForce(thisFace, &totalForce, &closestOnThisFace);
}

#endif // FACEWORKER_CL
