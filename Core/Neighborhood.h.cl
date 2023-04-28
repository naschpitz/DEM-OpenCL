#ifndef NEIGHBORHOOD_H_CL
#define NEIGHBORHOOD_H_CL

#define MAX_PARTICLES_TO_PARTICLES 512
#define MAX_FACES_TO_PARTICLES 128

#define MAX_PARTICLES_TO_FACES 2048

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

#endif // NEIGHBORHOOD_H_CL
