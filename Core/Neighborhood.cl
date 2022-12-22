#ifndef NEIGHBORHOOD_CL
#define NEIGHBORHOOD_CL

#define MAX_PARTICLES_TO_PARTICLES 128
#define MAX_FACES_TO_PARTICLES 16

#define MAX_PARTICLES_TO_FACES 512

typedef struct
{
    uint numParticles;
    uint numFaces;

    uint particles[MAX_PARTICLES_TO_PARTICLES];
    uint faces[MAX_FACES_TO_PARTICLES];
} ParticleNeighborhood;

typedef struct
{
    uint numParticles;

    uint particles[MAX_PARTICLES_TO_FACES];
} FaceNeighborhood;

void neighborhood_addParticleToParticleNeighborhood(ParticleNeighborhood* particleNeighborhood, const Particle* particle)
{
    if(particleNeighborhood->numParticles == MAX_PARTICLES_TO_PARTICLES) {
        printf("MAX_PARTICLES_TO_PARTICLES reached!\n");
        return;
    }

    uint numParticles = ++particleNeighborhood->numParticles;
    particleNeighborhood->particles[numParticles - 1] = particle->index;
}

void neighborhood_addFaceToParticleNeighborhood(ParticleNeighborhood* particleNeighborhood, const Face* face)
{
    if(particleNeighborhood->numParticles == MAX_FACES_TO_PARTICLES) {
        printf("MAX_FACES_TO_PARTICLES reached!\n");
        return;
    }

    uint numFaces = ++particleNeighborhood->numFaces;
    particleNeighborhood->faces[numFaces - 1] = face->index;
}

void neighborhood_addParticleToFaceNeighborhood(FaceNeighborhood* faceNeighborhood, const Particle* particle)
{
    if(faceNeighborhood->numParticles == MAX_PARTICLES_TO_FACES) {
        printf("MAX_PARTICLES_TO_FACES reached!\n");
        return;
    }

    uint numParticles = ++faceNeighborhood->numParticles;
    faceNeighborhood->particles[numParticles - 1] = particle->index;
}

bool neighborhood_isParticleNeighborToParticle(const ParticleNeighborhood* particleNeighborhood, const Particle* particle)
{
    for(uint i = 0; i < particleNeighborhood->numParticles; i++) {
        if (particleNeighborhood->particles[i] == particle->index)
            return true;
    }

    return false;
}

bool neighborhood_isParticleNeighborToFace(const ParticleNeighborhood* particleNeighborhood, const Face* face)
{
    for(uint i = 0; i < particleNeighborhood->numFaces; i++) {
        if (particleNeighborhood->faces[i] == face->index)
            return true;
    }

    return false;
}

bool neighborhood_isFaceNeighborToParticle(const FaceNeighborhood* faceNeighborhood, const Particle* particle)
{
    for(uint i = 0; i < faceNeighborhood->numParticles; i++) {
        if (faceNeighborhood->particles[i] == particle->index)
            return true;
    }

    return false;
}

#endif // NEIGHBORHOOD_CL
