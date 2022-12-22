#ifndef NEIGHBORHOOD_CL
#define NEIGHBORHOOD_CL

#define MAX_PARTICLES 131072
#define MAX_FACES 65536

#include "Face.cl"
#include "Particle.cl"

typedef struct
{
    // bool has actually the size of an unsigned int, 32 bits.
    bool particles[MAX_PARTICLES/32];
} ParticleNeighborhood;

typedef struct
{
    // bool has actually the size of an unsigned int, 32 bits.
    bool faces[MAX_FACES/32];
} FaceNeighborhood;

bool neighborhood_isNeighborToParticle(const ParticleNeighborhood* particleNeighborhood, const Particle* particle)
{
    uint  index = particle->index / 32;
    uchar remainder = particle->index % 32;

    return particleNeighborhood->particles[index] & (1 << remainder);
}

bool neighborhood_isNeighborToFace(const FaceNeighborhood* faceNeighborhood, const Face* face)
{
    uint  index = face->index / 32;
    uchar remainder = face->index % 32;

    return faceNeighborhood->faces[index] & (1 << remainder);
}

void neighborhood_setNeighborToParticle(ParticleNeighborhood* particleNeighborhood, const Particle* particle, bool isNeighbor)
{
    uint  index = particle->index / 32;
    uchar remainder = particle->index % 32;

    if (isNeighbor)
        particleNeighborhood->particles[index] |= 1 << remainder;
    else
        particleNeighborhood->particles[index] &= ~(1 << remainder);
}

void neighborhood_setNeighborToFace(FaceNeighborhood* faceNeighborhood, const Face* face, bool isNeighbor)
{
    uint  index = face->index / 32;
    uchar remainder = face->index % 32;

    if (isNeighbor)
        faceNeighborhood->faces[index] |= 1 << remainder;
    else
        faceNeighborhood->faces[index] &= ~(1 << remainder);
}

#endif // NEIGHBORHOOD_CL
