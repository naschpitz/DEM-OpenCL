#ifndef NEIGHBORHOOD_CPP_CL
#define NEIGHBORHOOD_CPP_CL

#include "../Face.h.cl"
#include "../Neighborhood.h.cl"
#include "../Particle.h.cl"

void neighborhood_addParticleToParticleNeighborhood(Particle* thisParticle, const Particle* otherParticle, global ParticleNeighborhood* particlesNeighborhood)
{
    uint thisParticleIndex = thisParticle->index;
    uint otherParticleIndex = otherParticle->index;

    global ParticleNeighborhood* thisParticleNeighborhood = &particlesNeighborhood[thisParticleIndex];

    uint numParticles = thisParticleNeighborhood->numParticles;

    if(numParticles == MAX_PARTICLES_TO_PARTICLES) {
        printf("MAX_PARTICLES_TO_PARTICLES reached!\n");
        return;
    }

    thisParticleNeighborhood->particles[numParticles].index = otherParticleIndex;
    thisParticleNeighborhood->numParticles++;
}

void neighborhood_addFaceToParticleNeighborhood(Particle* thisParticle, const Face* otherFace, global ParticleNeighborhood* particlesNeighborhood)
{
    uint thisParticleIndex = thisParticle->index;
    uint otherFaceIndex = otherFace->index;

    global ParticleNeighborhood* thisParticleNeighborhood = &particlesNeighborhood[thisParticleIndex];

    uint numFaces = thisParticleNeighborhood->numFaces;

    if(numFaces == MAX_FACES_TO_PARTICLES) {
        printf("MAX_FACES_TO_PARTICLES reached!\n");
        return;
    }

    thisParticleNeighborhood->faces[numFaces].index = otherFaceIndex;
    thisParticleNeighborhood->numFaces++;
}

void neighborhood_addParticleToFaceNeighborhood(Face* thisFace, const Particle* otherParticle, global FaceNeighborhood* facesNeighborhood)
{
    uint thisFaceIndex = thisFace->index;
    uint otherParticleIndex = otherParticle->index;

    global FaceNeighborhood* thisFaceNeighborhood = &facesNeighborhood[thisFaceIndex];

    uint numParticles = thisFaceNeighborhood->numParticles;

    if(numParticles == MAX_PARTICLES_TO_FACES) {
        printf("MAX_PARTICLES_TO_FACES reached!\n");
        return;
    }

    thisFaceNeighborhood->particles[numParticles].index = otherParticleIndex;
    thisFaceNeighborhood->numParticles++;
}

void neighborhood_resetParticleNeighborhood(Particle* thisParticle, global ParticleNeighborhood* particlesNeighborhood)
{ 
    uint thisParticleIndex = thisParticle->index;

    global ParticleNeighborhood* thisParticleNeighborhood = &particlesNeighborhood[thisParticleIndex];

    thisParticleNeighborhood->numParticles = 0;
    thisParticleNeighborhood->numFaces = 0;
}

void neighborhood_resetFaceNeighborhood(Face* thisFace, global FaceNeighborhood* facesNeighborhood)
{
    uint thisFaceIndex = thisFace->index;

    global FaceNeighborhood* thisFaceNeighborhood = &facesNeighborhood[thisFaceIndex];

    thisFaceNeighborhood->numParticles = 0;
}

#endif // NEIGHBORHOOD_CPP_CL
