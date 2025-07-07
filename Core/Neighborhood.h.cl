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

// ParticleNeighborhood and FaceNeighborhood structs removed
// Now using separate buffers for better memory management

#endif // NEIGHBORHOOD_H_CL
