#ifndef SIMULATION_CL
#define SIMULATION_CL

#include "../Face.cl"
#include "../FaceWorker.cl"
#include "../MaterialsManager.cl"
#include "../Particle.cl"
#include "../ParticleWorker.cl"
#include "../ReducedParticle.cl"
#include "../ReducedVertex.cl"
#include "../Scenery.cl"

typedef struct
{
    double currentTime;
    double timeStep;
    double totalTime;
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

kernel void calculate_particle_to_particle(global Particle* particles, constant MaterialsManager* ptrMaterialsManager, constant Scenery* ptrScenery)
{
    size_t idx  = get_global_id(0);
    size_t size = get_global_size(0);

    size_t lidx = get_local_id(0);
    size_t lsize = get_local_size(0);

    size_t ngroups = size / lsize;

    __local ReducedParticle reducedParticlesCache[256];

    Particle thisParticle = particles[idx];
    MaterialsManager materialsManager = ptrMaterialsManager[0];
    Scenery scenery = ptrScenery[0];

    thisParticle.currentForce = (0, 0, 0, 0);
    thisParticle.currentTorque = (0, 0, 0, 0);

    /*
    for(size_t i = 0; i < ngroups; i++) {
        Particle particle = particles[i*lsize + lidx];
        reducedParticlesCache[lidx] = reducedParticle_fromParticle(&particle);
        barrier(CLK_LOCAL_MEM_FENCE);

        for(size_t j = 0; j < lsize; j++) {
            ReducedParticle reducedParticle = reducedParticlesCache[j];
            Particle otherParticle = reducedParticle_toParticle(&reducedParticle);

            if (thisParticle.index == otherParticle.index)
                continue;

            int thisMaterialIndex = thisParticle.materialIndex;
            int otherMateriaIndex = otherParticle.materialIndex;

            const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

            particleToParticleWorker_run(&thisParticle, &otherParticle, material);
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    particles[idx] = thisParticle;
*/

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

kernel void calculate_particle_to_face(global Particle* particles, global Face* faces, constant MaterialsManager* ptrMaterialsManager, constant Scenery* ptrScenery)
{
    size_t idx  = get_global_id(0);

    Particle thisParticle = particles[idx];
    MaterialsManager materialsManager = ptrMaterialsManager[0];
    Scenery scenery = ptrScenery[0];

    for(ulong i = 0; i < scenery.numFaces; i++) {
        Face otherFace = faces[i];

        int thisMaterialIndex = thisParticle.materialIndex;
        int otherMateriaIndex = otherFace.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        particleToFaceWorker_run(&thisParticle, &otherFace, material);
    }

    particles[idx] = thisParticle;
}

kernel void calculate_face_to_particle(global Face* faces, global Particle* particles, constant MaterialsManager* ptrMaterialsManager, constant Scenery* ptrScenery)
{
    size_t idx  = get_global_id(0);

    Face thisFace = faces[idx];
    MaterialsManager materialsManager = ptrMaterialsManager[0];
    Scenery scenery = ptrScenery[0];

    for(ulong i = 0; i < scenery.numParticles; i++) {
        Particle otherParticle = particles[i];

        int thisMaterialIndex = thisFace.materialIndex;
        int otherMateriaIndex = otherParticle.materialIndex;

        const Material* material = materialsManager_getMaterial(thisMaterialIndex, otherMateriaIndex, &materialsManager);

        faceToParticleWorker_run(&thisFace, &otherParticle, material);
    }

    faces[idx] = thisFace;
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

#endif // SIMULATION_CL
