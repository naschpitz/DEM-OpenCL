#ifndef NEIGHBORHOOD_CPP_CL
#define NEIGHBORHOOD_CPP_CL

#include "../Face.h.cl"
#include "../Neighborhood.h.cl"
#include "../Particle.h.cl"

void neighborhood_addParticleToParticleNeighborhood(Particle* thisParticle, const Particle* otherParticle)
{
    if(thisParticle->neighborhood.numParticles == MAX_PARTICLES_TO_PARTICLES) {
        printf("MAX_PARTICLES_TO_PARTICLES reached!\n");
        return;
    }

    uint numParticles = ++thisParticle->neighborhood.numParticles;
    thisParticle->neighborhood.particles[numParticles - 1] = otherParticle->index;
}

void neighborhood_addFaceToParticleNeighborhood(Particle* thisParticle, const Face* otherFace)
{
    if(thisParticle->neighborhood.numFaces == MAX_FACES_TO_PARTICLES) {
        printf("MAX_FACES_TO_PARTICLES reached!\n");
        return;
    }

    uint numFaces = ++thisParticle->neighborhood.numFaces;
    thisParticle->neighborhood.faces[numFaces - 1] = otherFace->index;
}

void neighborhood_addParticleToFaceNeighborhood(Face* thisFace, const Particle* otherParticle)
{
    if(thisFace->neighborhood.numParticles == MAX_PARTICLES_TO_FACES) {
        printf("MAX_PARTICLES_TO_FACES reached!\n");
        return;
    }

    uint numParticles = ++thisFace->neighborhood.numParticles;
    thisFace->neighborhood.particles[numParticles - 1] = otherParticle->index;
}

void neighborhood_resetParticleNeighborhood(Particle* thisParticle)
{
    thisParticle->neighborhood.numParticles = 0;
    thisParticle->neighborhood.numFaces = 0;
}

void neighborhood_resetFaceNeighborhood(Face* thisFace)
{
    thisFace->neighborhood.numParticles = 0;
}

#endif // NEIGHBORHOOD_CPP_CL
