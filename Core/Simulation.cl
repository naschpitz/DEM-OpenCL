#ifndef SIMULATION_CL
#define SIMULATION_CL

#include "../Face.cl"
#include "../FaceWorker.cl"
#include "../Particle.cl"
#include "../ParticleWorker.cl"
#include "../MaterialsManager.cl"

typedef struct
{
    ulong numParticles;
    ulong numFaces;

    double currentTime;
    double timeStep;
    double totalTime;

    double4 gravity;
} Simulation;

kernel void initialize_particles(global Particle* particles, constant Simulation* ptrSimulation)
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

kernel void initialize_faces(global Face* faces, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    Face thisFace = faces[idx];
    Simulation simulation = ptrSimulation[0];

    for(int i = 0; i < 3; i++) {
        thisFace.vertexes[i].oldPosition = thisFace.vertexes[i].originalPosition - thisFace.vertexes[i].originalVelocity * (simulation.timeStep);
        thisFace.vertexes[i].currentPosition = thisFace.vertexes[i].originalPosition;
    }

    thisFace.currentForce = (0, 0, 0, 0);
    thisFace.oldForce     = (0, 0, 0, 0);

    thisFace.currentTorque = (0, 0, 0, 0);
    thisFace.oldTorque     = (0, 0, 0, 0);

    faces[idx] = thisFace;
}

kernel void calculate_particle_to_particle(global Particle* particles, constant MaterialsManager* ptrMaterialsManager, constant Simulation* ptrSimulation)
{
    size_t idx  = get_global_id(0);
    size_t size = get_global_size(0);

    Particle thisParticle = particles[idx];
    MaterialsManager materialsManager = ptrMaterialsManager[0];
    Simulation simulation = ptrSimulation[0];

    thisParticle.currentForce = (0, 0, 0, 0);
    thisParticle.currentTorque = (0, 0, 0, 0);

    for(ulong i = 0; i < size; i++) {
        if(i == idx)
            continue;

        Particle otherParticle = particles[i];

        int thisMaterialIndex = thisParticle.materialIndex;
        int otherMateriaIndex = otherParticle.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        particleToParticleWorker_run(&thisParticle, &otherParticle, material);
    }

    particles[idx] = thisParticle;
}

kernel void calculate_particle_to_face(global Particle* particles, constant Face* faces, constant MaterialsManager* ptrMaterialsManager, constant Simulation* ptrSimulation)
{
    size_t idx  = get_global_id(0);

    Particle thisParticle = particles[idx];
    MaterialsManager materialsManager = ptrMaterialsManager[0];
    Simulation simulation = ptrSimulation[0];

    ulong numFaces = simulation.numFaces;

    thisParticle.currentForce = (0, 0, 0, 0);
    thisParticle.currentTorque = (0, 0, 0, 0);

    for(ulong i = 0; i < numFaces; i++) {
        Face otherFace = faces[i];

        int thisMaterialIndex = thisParticle.materialIndex;
        int otherMateriaIndex = otherFace.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        particleToFaceWorker_run(&thisParticle, &otherFace, material);
    }

    particles[idx] = thisParticle;
}

kernel void calculate_face_to_particle(global Face* faces, constant Particle* particles, constant MaterialsManager* ptrMaterialsManager, constant Simulation* ptrSimulation)
{
    size_t idx  = get_global_id(0);

    Face thisFace = faces[idx];
    MaterialsManager materialsManager = ptrMaterialsManager[0];
    Simulation simulation = ptrSimulation[0];

    ulong numParticles = simulation.numParticles;

    thisFace.currentForce = (0, 0, 0, 0);
    thisFace.currentTorque = (0, 0, 0, 0);

    for(ulong i = 0; i < numParticles; i++) {
        Particle otherParticle = particles[i];

        int thisMaterialIndex = thisFace.materialIndex;
        int otherMateriaIndex = otherParticle.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        faceToParticleWorker_run(&thisFace, &otherParticle, material);
    }

    faces[idx] = thisFace;
}

kernel void apply_particles_gravity(global Particle* particles, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    Particle particle = particles[idx];
    Simulation simulation = ptrSimulation[0];

    particle.currentForce += particle.mass * simulation.gravity;

    particles[idx] = particle;
}

kernel void apply_faces_gravity(global Face* faces, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    Face face = faces[idx];
    Simulation simulation = ptrSimulation[0];

    face.currentForce += face.mass * simulation.gravity;

    faces[idx] = face;
}

kernel void integrate_particles(global Particle* particles, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    Particle particle = particles[idx];
    Simulation simulation = ptrSimulation[0];

    particle_integrate(&particle, simulation.timeStep);

    particles[idx] = particle;
}

kernel void integrate_faces(global Face* faces, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    Face face = faces[idx];
    Simulation simulation = ptrSimulation[0];

    face_integrate(&face, simulation.timeStep);

    faces[idx] = face;
}

#endif // SIMULATION_CL
