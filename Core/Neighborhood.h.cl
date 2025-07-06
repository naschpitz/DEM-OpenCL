#ifndef NEIGHBORHOOD_H_CL
#define NEIGHBORHOOD_H_CL

#define MAX_PARTICLES_TO_PARTICLES 1024
#define MAX_FACES_TO_PARTICLES 512

#define MAX_PARTICLES_TO_FACES 2048
#define MAX_FACES_TO_FACES 256

typedef struct
{
    uint index;
    float4 currentForce;
    float4 currentTorque;
} ForceTorque;

typedef struct
{
    uint numParticles;
    uint numFaces;

    ForceTorque particles[MAX_PARTICLES_TO_PARTICLES];
    ForceTorque faces[MAX_FACES_TO_PARTICLES];
} ParticleNeighborhood;

typedef struct
{
    uint numParticles;

    ForceTorque particles[MAX_PARTICLES_TO_FACES];
} FaceNeighborhood;

#endif // NEIGHBORHOOD_H_CL
