#ifndef SIMULATION_CPP_CL
#define SIMULATION_CPP_CL

#include "../Scenery.h.cl"
#include "../Simulation.h.cl"

#include "../Face.cpp.cl"
#include "../FaceWorker.cpp.cl"
#include "../MaterialsManager.cpp.cl"
#include "../Neighborhood.cpp.cl"
#include "../Particle.cpp.cl"
#include "../ParticleWorker.cpp.cl"

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

    thisParticle.currentForce = (0, 0, 0, 0);
    thisParticle.oldForce     = (0, 0, 0, 0);

    thisParticle.currentTorque = (0, 0, 0, 0);
    thisParticle.oldTorque     = (0, 0, 0, 0);

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

    thisFace.currentForce = (0, 0, 0, 0);
    thisFace.oldForce     = (0, 0, 0, 0);

    thisFace.currentTorque = (0, 0, 0, 0);
    thisFace.oldTorque     = (0, 0, 0, 0);

    faces[idx] = thisFace;
}

kernel void calculate_particles_neighborhood(global Particle* particles, global Face* faces, constant MaterialsManager* ptrMaterialsManager, constant Scenery* ptrScenery, constant Simulation* ptrSimulation, global ParticleNeighborhood* particlesNeighborhood)
{
    Simulation simulation = ptrSimulation[0];

    if((simulation.currentStep % simulation.calcNeighStepsInt) != 0 && simulation.currentStep != 0)
        return;

    size_t idx = get_global_id(0);

    Particle thisParticle = particles[idx];
    MaterialsManager materialsManager = ptrMaterialsManager[0];
    Scenery scenery = ptrScenery[0];

    neighborhood_resetParticleNeighborhood(&thisParticle, particlesNeighborhood);

    for(ulong i = 0; i < scenery.numParticles; i++) {
        Particle otherParticle = particles[i];

        if(thisParticle.index >= otherParticle.index)
            continue;

        int thisMaterialIndex = thisParticle.materialIndex;
        int otherMateriaIndex = otherParticle.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        if(!testBox_particleToParticle(&thisParticle, &otherParticle, material->distanceThreshold * simulation.neighDistThresMult))
            continue;

        neighborhood_addParticleToParticleNeighborhood(&thisParticle, &otherParticle, particlesNeighborhood);
    }

    for(ulong i = 0; i < scenery.numFaces; i++) {
        Face otherFace = faces[i];

        int thisMaterialIndex = thisParticle.materialIndex;
        int otherMateriaIndex = otherFace.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        if(!testBox_particleToFace(&thisParticle, &otherFace, material->distanceThreshold))
            continue;

        neighborhood_addFaceToParticleNeighborhood(&thisParticle, &otherFace, particlesNeighborhood);
    }

    particles[idx] = thisParticle;
}

kernel void calculate_faces_neighborhood(global Face* faces, global Particle* particles, constant MaterialsManager* ptrMaterialsManager, constant Scenery* ptrScenery, constant Simulation* ptrSimulation, global FaceNeighborhood* facesNeighborhood)
{
    Simulation simulation = ptrSimulation[0];

    if((simulation.currentStep % simulation.calcNeighStepsInt) != 0 && simulation.currentStep != 0)
        return;

    size_t idx = get_global_id(0);

    Face thisFace = faces[idx];
    MaterialsManager materialsManager = ptrMaterialsManager[0];
    Scenery scenery = ptrScenery[0];

    neighborhood_resetFaceNeighborhood(&thisFace, facesNeighborhood);

    for(ulong i = 0; i < scenery.numParticles; i++) {
        Particle otherParticle = particles[i];

        int thisMaterialIndex = thisFace.materialIndex;
        int otherMateriaIndex = otherParticle.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        if(!testBox_particleToFace(&otherParticle, &thisFace, material->distanceThreshold * simulation.neighDistThresMult))
            continue;

        neighborhood_addParticleToFaceNeighborhood(&thisFace, &otherParticle, facesNeighborhood);
    }

    faces[idx] = thisFace;
}

kernel void calculate_particle_to_particle(global Particle* particles, constant MaterialsManager* ptrMaterialsManager, global ParticleNeighborhood* particlesNeighborhood)
{
    size_t thisParticleIndex = get_global_id(0);

    Particle thisParticle = particles[thisParticleIndex];
    MaterialsManager materialsManager = ptrMaterialsManager[0];

    global ParticleNeighborhood* thisParticleNeighborhood = &particlesNeighborhood[thisParticleIndex];

    uint numParticles = thisParticle.neighborhood.numParticles;

    for(ulong i = 0; i < numParticles; i++) {
        size_t otherParticleIndex = thisParticleNeighborhood->particles[i].index;

        Particle otherParticle = particles[otherParticleIndex];

        if(thisParticleIndex >= otherParticleIndex)
            continue;

        int thisMaterialIndex = thisParticle.materialIndex;
        int otherMateriaIndex = otherParticle.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        particleToParticleWorker_run(&thisParticle, &otherParticle, material, particlesNeighborhood);
    }

    particles[thisParticleIndex] = thisParticle;
}

kernel void calculate_particle_to_face(global Particle* particles, global Face* faces, constant MaterialsManager* ptrMaterialsManager, global ParticleNeighborhood* particlesNeighborhood, global FaceNeighborhood* facesNeighborhood)
{
    size_t thisParticleIndex  = get_global_id(0);

    Particle thisParticle = particles[thisParticleIndex];
    MaterialsManager materialsManager = ptrMaterialsManager[0];

    global ParticleNeighborhood* thisParticleNeighborhood = &particlesNeighborhood[thisParticleIndex];

    uint numFaces = thisParticle.neighborhood.numFaces;

    for(ulong i = 0; i < numFaces; i++) {
        size_t otherFaceIndex = thisParticleNeighborhood->faces[i].index;

        Face otherFace = faces[otherFaceIndex];

        int thisMaterialIndex = thisParticle.materialIndex;
        int otherMateriaIndex = otherFace.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        particleToFaceWorker_run(&thisParticle, &otherFace, material, numFaces, facesNeighborhood);
    }

    particles[thisParticleIndex] = thisParticle;
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

kernel void integrate_particles(global Particle* particles, global ParticleNeighborhood* particlesNeighborhood, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    Particle particle = particles[idx];
    Simulation simulation = ptrSimulation[0];

    particle_sumForces(&particle, particlesNeighborhood);
    particle_integrate(&particle, simulation.timeStep);

    particles[idx] = particle;
}

kernel void integrate_faces(global Face* faces, global FaceNeighborhood* facesNeighborhood, constant Simulation* ptrSimulation)
{
    size_t idx = get_global_id(0);

    Face face = faces[idx];
    Simulation simulation = ptrSimulation[0];

    face_sumForces(&face, facesNeighborhood);
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
