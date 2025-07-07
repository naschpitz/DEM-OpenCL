#ifndef NEIGHBORHOOD_CPP_CL
#define NEIGHBORHOOD_CPP_CL

#include "../Face.h.cl"
#include "../Neighborhood.h.cl"
#include "../Particle.h.cl"

void neighborhood_addParticleToParticleNeighborhood(Particle* thisParticle, const Particle* otherParticle, global ForceTorque* particlesToParticlesNeighborhood, global uint* particlesToParticlesNeighborhoodNum)
{
    uint thisParticleIndex = thisParticle->index;
    uint otherParticleIndex = otherParticle->index;

    uint numParticles = particlesToParticlesNeighborhoodNum[thisParticleIndex];

    if(numParticles >= MAX_PARTICLES_TO_PARTICLES) {
        printf("MAX_PARTICLES_TO_PARTICLES reached!\n");
        return;
    }

    uint neighborIndex = thisParticleIndex * MAX_PARTICLES_TO_PARTICLES + numParticles;
    particlesToParticlesNeighborhood[neighborIndex].index = otherParticleIndex;
    particlesToParticlesNeighborhoodNum[thisParticleIndex]++;
}

void neighborhood_addFaceToParticleNeighborhood(Particle* thisParticle, const Face* otherFace, global ForceTorque* facesToParticlesNeighborhood, global uint* facesToParticlesNeighborhoodNum)
{
    uint thisParticleIndex = thisParticle->index;
    uint otherFaceIndex = otherFace->index;

    uint numFaces = facesToParticlesNeighborhoodNum[thisParticleIndex];

    if(numFaces >= MAX_FACES_TO_PARTICLES) {
        printf("MAX_FACES_TO_PARTICLES reached!\n");
        return;
    }

    uint neighborIndex = thisParticleIndex * MAX_FACES_TO_PARTICLES + numFaces;
    facesToParticlesNeighborhood[neighborIndex].index = otherFaceIndex;
    facesToParticlesNeighborhoodNum[thisParticleIndex]++;
}

void neighborhood_addParticleToFaceNeighborhood(Face* thisFace, const Particle* otherParticle, global ForceTorque* particlesToFacesNeighborhood, global uint* particlesToFacesNeighborhoodNum)
{
    uint thisFaceIndex = thisFace->index;
    uint otherParticleIndex = otherParticle->index;

    uint numParticles = particlesToFacesNeighborhoodNum[thisFaceIndex];

    if(numParticles >= MAX_PARTICLES_TO_FACES) {
        printf("MAX_PARTICLES_TO_FACES reached!\n");
        return;
    }

    uint neighborIndex = thisFaceIndex * MAX_PARTICLES_TO_FACES + numParticles;
    particlesToFacesNeighborhood[neighborIndex].index = otherParticleIndex;
    particlesToFacesNeighborhoodNum[thisFaceIndex]++;
}

void neighborhood_resetParticleNeighborhood(Particle* thisParticle, global uint* particlesToParticlesNeighborhoodNum, global uint* facesToParticlesNeighborhoodNum)
{
    uint thisParticleIndex = thisParticle->index;

    particlesToParticlesNeighborhoodNum[thisParticleIndex] = 0;
    facesToParticlesNeighborhoodNum[thisParticleIndex] = 0;
}

void neighborhood_resetFaceNeighborhood(Face* thisFace, global uint* particlesToFacesNeighborhoodNum)
{
    uint thisFaceIndex = thisFace->index;

    particlesToFacesNeighborhoodNum[thisFaceIndex] = 0;
}

#endif // NEIGHBORHOOD_CPP_CL
