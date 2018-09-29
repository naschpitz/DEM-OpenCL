#ifndef SIMULATION_CL
#define SIMULATION_CL

#include "../Particle.cl"
#include "../ParticleWorker.cl"
#include "../MaterialsManager.cl"

typedef struct
{
    double currentTime;
    double timeStep;
    double totalTime;

    double4 gravity;
} Simulation;

kernel void initialize(global Particle* particles, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    Particle thisParticle = particles[idx];
    Simulation simulation = ptrSimulation[0];

    thisParticle.vertex.oldPosition = thisParticle.vertex.originalPosition - thisParticle.vertex.originalVelocity * (simulation.timeStep);
    thisParticle.vertex.currentPosition = thisParticle.vertex.originalPosition;

    thisParticle.currentForce = (0, 0, 0, 0);
    thisParticle.oldForce     = (0, 0, 0, 0);

    thisParticle.currentTorque = (0, 0, 0, 0);
    thisParticle.oldTorque     = (0, 0, 0, 0);

    particles[idx] = thisParticle;
}

kernel void calculate_forces(global Particle* particles, constant MaterialsManager* ptrMaterialsManager, constant Simulation* ptrSimulation)
{
    size_t idx  = get_global_id(0);
    size_t size = get_global_size(0);

    MaterialsManager materialsManager = ptrMaterialsManager[0];
    Simulation simulation = ptrSimulation[0];
    Particle thisParticle = particles[idx];

    thisParticle.currentForce = (0, 0, 0, 0);
    thisParticle.currentTorque = (0, 0, 0, 0);

    for(ulong i = 0; i < size; i++) {
        if(i == idx) {
            thisParticle.currentForce += thisParticle.mass * simulation.gravity;
            continue;
        }

        Particle otherParticle = particles[i];

        int thisMaterialIndex = thisParticle.materialIndex;
        int otherMateriaIndex = otherParticle.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        particleWorker_run(&thisParticle, &otherParticle, material);
    }

    particles[idx] = thisParticle;
}

kernel void integrate(global Particle* particles, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    Particle particle = particles[idx];
    Simulation simulation = ptrSimulation[0];

    particle_integrate(&particle, simulation.timeStep);

    particles[idx] = particle;
}

#endif // SIMULATION_CL
