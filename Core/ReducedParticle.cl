#ifndef REDUCEDPARTICLE_CL
#define REDUCEDPARTICLE_CL

#include "../Vector.cl"
#include "../Particle.cl"
#include "../ReducedVertex.cl"

typedef struct
{
    uint index;
    uint materialIndex;

    float radius;
    float area;

    ReducedVertex vertex;
} ReducedParticle;

ReducedParticle reducedParticle_fromParticle(const Particle* particle)
{
    ReducedParticle reducedParticle;

    reducedParticle.index = particle->index;
    reducedParticle.materialIndex = particle->materialIndex;

    reducedParticle.radius = particle->radius;
    reducedParticle.area = particle->area;

    reducedParticle.vertex = reducedVertex_fromVertex(&(particle->vertex));

    return reducedParticle;
}

Particle reducedParticle_toParticle(const ReducedParticle* reducedParticle)
{
    Particle particle;

    particle.index = reducedParticle->index;
    particle.materialIndex = reducedParticle->materialIndex;

    particle.radius = reducedParticle->radius;
    particle.area = reducedParticle->area;

    particle.vertex = reducedVertex_toVertex(&(reducedParticle->vertex));

    return particle;
}

#endif // REDUCEDPARTICLE_CL
