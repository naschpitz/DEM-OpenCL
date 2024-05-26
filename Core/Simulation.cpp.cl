#ifndef SIMULATION_CPP_CL
#define SIMULATION_CPP_CL

#include "../Pair.h.cl"
#include "../Scenery.h.cl"
#include "../Simulation.h.cl"

#include "../Face.cpp.cl"
#include "../MaterialsManager.cpp.cl"
#include "../Particle.cpp.cl"
#include "../TestBox.cpp.cl"
#include "../Workers.cpp.cl"

kernel void initialize_particles(global Particle* particles, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    if(idx == 0)
        printf("Initializing particles.\n");

    Particle thisParticle = particles[idx];
    Simulation simulation = ptrSimulation[0];

    thisParticle.vertex.oldPosition = thisParticle.vertex.originalPosition - thisParticle.vertex.originalVelocity * (simulation.timeStep);
    thisParticle.vertex.currentPosition = thisParticle.vertex.originalPosition;

    thisParticle.vertex.preciseOldPosition = convert_double4(thisParticle.vertex.oldPosition);
    thisParticle.vertex.preciseCurrentPosition = convert_double4(thisParticle.vertex.currentPosition);

    thisParticle.currentForceX = thisParticle.currentForceY = thisParticle.currentForceZ = 0;

    thisParticle.currentForce = (float4)(0, 0, 0, 0);
    thisParticle.oldForce     = (float4)(0, 0, 0, 0);

    thisParticle.currentTorqueX = thisParticle.currentTorqueY = thisParticle.currentTorqueZ = 0;

    thisParticle.currentTorque = (float4)(0, 0, 0, 0);
    thisParticle.oldTorque     = (float4)(0, 0, 0, 0);

    particles[idx] = thisParticle;
}

kernel void initialize_faces(global Face* faces, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    if(idx == 0)
        printf("Initializing faces.\n");

    Face thisFace = faces[idx];
    Simulation simulation = ptrSimulation[0];

    for(int i = 0; i < 3; i++) {
        thisFace.vertexes[i].oldPosition = thisFace.vertexes[i].originalPosition - thisFace.vertexes[i].originalVelocity * (simulation.timeStep);
        thisFace.vertexes[i].currentPosition = thisFace.vertexes[i].originalPosition;

        thisFace.vertexes[i].preciseOldPosition = convert_double4(thisFace.vertexes[i].oldPosition);
        thisFace.vertexes[i].preciseCurrentPosition = convert_double4(thisFace.vertexes[i].currentPosition);
    }

    face_calculateCurrentPosition(&thisFace);
    face_calculateCurrentVelocity(&thisFace);

    thisFace.currentForceX = thisFace.currentForceY = thisFace.currentForceZ = 0;

    thisFace.currentForce = (float4)(0, 0, 0, 0);
    thisFace.oldForce     = (float4)(0, 0, 0, 0);

    thisFace.currentTorqueX = thisFace.currentTorqueY = thisFace.currentTorqueZ = 0;

    thisFace.currentTorque = (float4)(0, 0, 0, 0);
    thisFace.oldTorque     = (float4)(0, 0, 0, 0);

    faces[idx] = thisFace;
}

kernel void calculate_particle_to_particle_neighborhood(global Pair* pairs, global Particle* particles, constant MaterialsManager* ptrMaterialsManager, constant Simulation* ptrSimulation)
{
    Simulation simulation = ptrSimulation[0];

    if((simulation.currentStep % simulation.calcNeighStepsInt) != 0 && simulation.currentStep != 0)
        return;

    size_t idx = get_global_id(0);

    Pair* pair = &pairs[idx];

    const Particle* thisParticle = &particles[pair->first];
    const Particle* otherParticle = &particles[pair->second];
    MaterialsManager materialsManager = ptrMaterialsManager[0];

    int thisMaterialIndex = thisParticle->materialIndex;
    int otherMateriaIndex = otherParticle->materialIndex;

    const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

    pairs[idx].isNeighbor = testBox_particleToParticle(thisParticle, otherParticle, material->distanceThreshold * simulation.neighDistThresMult);
}

kernel void calculate_particle_to_face_neighborhood(global Pair* pairs, global Particle* particles, global Face* faces, constant MaterialsManager* ptrMaterialsManager, constant Simulation* ptrSimulation)
{
    Simulation simulation = ptrSimulation[0];

    if((simulation.currentStep % simulation.calcNeighStepsInt) != 0 && simulation.currentStep != 0)
        return;

    size_t idx = get_global_id(0);

    Pair* pair = &pairs[idx];

    const Particle* thisParticle = &particles[pair->first];
    const Face* otherFace = &faces[pair->second];
    MaterialsManager materialsManager = ptrMaterialsManager[0];

    int thisMaterialIndex = thisParticle->materialIndex;
    int otherMateriaIndex = otherFace->materialIndex;

    const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

    pairs[idx].isNeighbor = testBox_particleToFace(thisParticle, otherFace, material->distanceThreshold * simulation.neighDistThresMult);
}

kernel void calculate_particle_to_particle(global Pair* pairs, global Particle* particles, constant MaterialsManager* ptrMaterialsManager)
{
    size_t idx = get_global_id(0);

    const Pair* pair = &pairs[idx];

    if (!pair->isNeighbor)
        return;

    volatile global Particle* thisParticle = &particles[pair->first];
    volatile global Particle* otherParticle = &particles[pair->second];
    MaterialsManager materialsManager = ptrMaterialsManager[0];

    int thisMaterialIndex = thisParticle->materialIndex;
    int otherMateriaIndex = otherParticle->materialIndex;

    const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

    particleToParticleWorker_run(thisParticle, otherParticle, material);
}

kernel void calculate_particle_to_face(global Pair* pairs, global Particle* particles, global Face* faces, constant MaterialsManager* ptrMaterialsManager)
{
    size_t idx = get_global_id(0);

    const Pair* pair = &pairs[idx];

    if (!pair->isNeighbor)
        return;

    volatile global Particle* thisParticle = &particles[pair->first];
    volatile global Face* otherFace = &faces[pair->second];
    MaterialsManager materialsManager = ptrMaterialsManager[0];

    int thisMaterialIndex = thisParticle->materialIndex;
    int otherMateriaIndex = otherFace->materialIndex;

    const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

    particleToFaceWorker_run(thisParticle, otherFace, material);
}

kernel void apply_particles_gravity(global Particle* particles, constant Scenery* ptrScenery)
{
    size_t idx = get_global_id(0);

    Particle particle = particles[idx];
    Scenery scenery = ptrScenery[0];

    particle.currentForce += particle.mass * scenery.gravity;

    particles[idx] = particle;
}

kernel void apply_faces_gravity(global Face* faces, constant Scenery* ptrScenery)
{
    size_t idx = get_global_id(0);

    Face face = faces[idx];
    Scenery scenery = ptrScenery[0];

    face.currentForce += face.mass * scenery.gravity;

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

kernel void reset_particles_forces(global Particle* particles)
{
    size_t idx = get_global_id(0);

    Particle thisParticle = particles[idx];

    thisParticle.currentForce = (0, 0, 0, 0);
    thisParticle.oldForce     = (0, 0, 0, 0);

    thisParticle.currentTorque = (0, 0, 0, 0);
    thisParticle.oldTorque     = (0, 0, 0, 0);

    particles[idx] = thisParticle;
}

kernel void reset_faces_forces(global Face* faces)
{
    size_t idx = get_global_id(0);

    Face thisFace = faces[idx];

    thisFace.currentForce = (0, 0, 0, 0);
    thisFace.oldForce     = (0, 0, 0, 0);

    thisFace.currentTorque = (0, 0, 0, 0);
    thisFace.oldTorque     = (0, 0, 0, 0);

    faces[idx] = thisFace;
}

#endif // SIMULATION_CPP_CL
