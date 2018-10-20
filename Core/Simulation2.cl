#ifndef VERTEX_CL
#define VERTEX_CL

typedef struct
{
    bool fixed;

    double4 originalPosition;
    double4 originalVelocity;
    double4 acceleration;

    double4 currentPosition;
    double4 oldPosition;

    double4 currentVelocity;
    double4 oldVelocity;
} Vertex;


void vertex_integrate(Vertex* vertex, double timeStep)
{
    double4 tempPosition;

    // Stores the current position.
    tempPosition = vertex->currentPosition;

    if(vertex->fixed)
        vertex->currentPosition += vertex->currentVelocity * timeStep;

    else
    {
        // Calculates the new position based on Verlet integration method.
        vertex->currentPosition = 2 * vertex->currentPosition - vertex->oldPosition + vertex->acceleration * timeStep * timeStep;

        // Calculates the new speed.
        vertex->currentVelocity = (vertex->currentPosition - tempPosition) / timeStep;
    }

    // Recovers the position stored before and assigns it to the vertice's past position field.
    vertex->oldPosition = tempPosition;

    // Resets the acceleration.
    vertex->acceleration = 0;
}

#endif // VERTEX_CL

//-------------------------------------------------------------------------------------------------------------------------------------------//

#ifndef VECTOR_CL
#define VECTOR_CL

double4 vector_getUnitary(double4 vector)
{
    double vectorLength = length(vector);

    if (isequal(vectorLength, 0.0))
        return (double4)0;

    double4 unitary = vector / vectorLength;

    return unitary;
}

#endif // VECTOR_CL

//-------------------------------------------------------------------------------------------------------------------------------------------//

#ifndef PARTICLE_CL
#define PARTICLE_CL

typedef struct
{
    uint index;
    uint materialIndex;

    double radius;
    double density;
    double mass;
    double area;
    double volume;

    double4 currentForce;
    double4 oldForce;

    double4 currentTorque;
    double4 oldTorque;

    Vertex vertex;
} Particle;

void particle_addCurrentForce(Particle* particle, const double4* force, const double4* pointOfForce)
{
    particle->currentForce += (*force);

    double4 r = (*pointOfForce) - particle->vertex.currentPosition;
    particle->currentTorque += cross(r, (*force));
}

void particle_getClosestTo(const Particle* thisParticle, const Particle* otherParticle, double4 *closestOnThisPaticle, double4 *closestOnOtherParticle)
{
    double4 distance = otherParticle->vertex.currentPosition - thisParticle->vertex.currentPosition;
    double4 distanceUnitary = vector_getUnitary(distance);

    (*closestOnThisPaticle)   = thisParticle->vertex.currentPosition + distanceUnitary * thisParticle->radius;
    (*closestOnOtherParticle) = otherParticle->vertex.currentPosition - distanceUnitary * otherParticle->radius;
}

double4 particle_getCurrentAcceleration(const Particle* particle)
{
    return particle->currentForce / particle->mass;
}

bool particle_isInternal(const Particle* particle, const double4 vector)
{
    double4 distance = particle->vertex.currentPosition - vector;

    if(length(distance) < particle->radius)
        return true;

    return false;
}

void particle_integrate(Particle* particle, double timeStep)
{
    double4 currentAcceleration = particle_getCurrentAcceleration(particle);

    particle->vertex.acceleration = currentAcceleration;

    vertex_integrate(&(particle->vertex), timeStep);

    particle->oldForce = particle->currentForce;
    particle->oldTorque = particle->currentTorque;
}

#endif // PARTICLE_CL

//-------------------------------------------------------------------------------------------------------------------------------------------//

#ifndef EDGE_CL
#define EDGE_CL

typedef struct
{
    Vertex v1, v2;
} Edge;

double4 edge_getDistance(const Edge* edge)
{
    return edge->v2.currentPosition - edge->v1.currentPosition;
}

void edge_getClosestTo(const Edge* thisEdge, const double4* otherVector, double4* closestOnThisEdge)
{
    const double4 p1 = thisEdge->v1.currentPosition;
    const double4 p2 = thisEdge->v2.currentPosition;

    const double4 q = (*otherVector);

    double4 a = q - p1;
    double4 b = edge_getDistance(thisEdge);

    double u = dot(a, b) / dot(b, b);

    if(u <= 0)
        (*closestOnThisEdge) = p1;

    else if(u >= 1)
        (*closestOnThisEdge) = p2;

    else
        (*closestOnThisEdge) = p1 + u * b;
}

#endif // EDGE_CL

//-------------------------------------------------------------------------------------------------------------------------------------------//

#ifndef FACE_CL
#define FACE_CL

typedef struct
{
    uint index;
    uint materialIndex;

    double area;
    double mass;

    double4 currentPosition;
    double4 currentVelocity;

    double4 currentForce;
    double4 oldForce;

    double4 currentTorque;
    double4 oldTorque;

    Vertex vertexes[3];
} Face;

void face_addCurrentForce(Face* face, const double4* force, const double4* pointOfForce)
{
    face->currentForce += (*force);

    double4 r = (*pointOfForce) - face->currentPosition;
    face->currentTorque += cross(r, (*force));
}

void face_getClosestTo(const Face* thisFace, const Particle* otherParticle, double4* closestOnThisFace, double4* closestOnOtherParticle)
{
    double4 p1 = thisFace->vertexes[0].currentPosition;
    double4 p2 = thisFace->vertexes[1].currentPosition;
    double4 p3 = thisFace->vertexes[2].currentPosition;

    double4 q = otherParticle->vertex.currentPosition;

    double4 a =  q - p1;
    double4 b = p2 - p1;
    double4 c = p3 - p1;

    double ab = dot(a, b);
    double ac = dot(a, c);
    double bb = dot(b, b);
    double bc = dot(b, c);
    double cc = dot(c, c);

    double  den = bb * cc - bc * bc;
    double numU = ab * cc - ac * bc;
    double numV = ac * bb - ab * bc;

    double u = numU / den;
    double v = numV / den;

    Edge e1;
    e1.v1 = thisFace->vertexes[0];
    e1.v2 = thisFace->vertexes[1];

    Edge e2;
    e2.v1 = thisFace->vertexes[0];
    e2.v2 = thisFace->vertexes[2];

    Edge e3;
    e3.v1 = thisFace->vertexes[1];
    e3.v2 = thisFace->vertexes[2];

    if(v < 0)
        edge_getClosestTo(&e1, &q, closestOnThisFace);

    else if(u < 0)
        edge_getClosestTo(&e2, &q, closestOnThisFace);

    else if((u + v) > 1)
        edge_getClosestTo(&e3, &q, closestOnThisFace);

    else // Inside the triangle.
        (*closestOnThisFace) = p1 + u * edge_getDistance(&e1) + v * edge_getDistance(&e2);

    double4 distance = otherParticle->vertex.currentPosition - (*closestOnThisFace);
    double4 distanceUnitary = vector_getUnitary(distance);

    (*closestOnOtherParticle) = otherParticle->vertex.currentPosition - distanceUnitary * otherParticle->radius;
}

void face_calculateCurrentPosition(Face* face)
{
    face->currentPosition = (face->vertexes[0].currentPosition + face->vertexes[1].currentPosition + face->vertexes[2].currentPosition) / 3;
}

void face_calculateCurrentVelocity(Face* face)
{
    face->currentVelocity = (face->vertexes[0].currentVelocity + face->vertexes[1].currentVelocity + face->vertexes[2].currentVelocity) / 3;
}

double4 face_getCurrentAcceleration(Face* face)
{
    return face->currentForce / face->mass;
}

void face_integrate(Face* face, double timeStep)
{
    double4 currentAcceleration = face_getCurrentAcceleration(face);

    for(int i = 0; i < 3; i++) {
        face->vertexes[i].acceleration = currentAcceleration;
        vertex_integrate(face->vertexes + i, timeStep);
    }

    face->oldForce = face->currentForce;
    face->oldTorque = face->currentTorque;
}

#endif // FACE_CL

//-------------------------------------------------------------------------------------------------------------------------------------------//

#ifndef MATERIAL_CL
#define MATERIAL_CL

enum ForceType
{
    hooks_law,
    inverse_linear,
    inverse_quadratic,
    inverse_cubic,
    adiabatic_compression,
    realistic_material
};

enum DragForceType
{
    linear,
    quadratic,
    cubic
};

typedef struct
{
    int materialIndex1;
    int materialIndex2;

    double distanceThreshold;

    int forceType;
    int dragForceType;

    double coefficients[10];
    double dragCoefficients[10];
} Material;

double4 material_calculateForce(const Material* material, double4 distance, bool internal, double contactArea, double originalLength, double4 oldForce)
{
    double lengthDistance = length(distance);

    switch(material->forceType)
    {
        case hooks_law:
            return -(material->coefficients[0]) * distance;

        case inverse_linear:
            return (material->coefficients[0] / lengthDistance) * vector_getUnitary(distance);

        case inverse_quadratic:
            return (material->coefficients[0] / (lengthDistance * lengthDistance)) * vector_getUnitary(distance);

        case inverse_cubic:
            return (material->coefficients[0] / (lengthDistance * lengthDistance * lengthDistance)) * vector_getUnitary(distance);

        case adiabatic_compression:
        {
            double newRadius;

            if(internal)
                newRadius = material->coefficients[0] - lengthDistance / 2.;

            else
                newRadius = material->coefficients[0] + lengthDistance / 2.;

            double newRadius2 = newRadius * newRadius;

            double4 force = (material->coefficients[1] * 12.56637061 * newRadius2 * pow(pown(material->coefficients[0], 3) / pown(newRadius, 3), material->coefficients[2])) * vector_getUnitary(distance);

            if(internal)
                return -force;

            return force;
        }

        case realistic_material:
        {
            // Rupture regime:
            double ruptureDistance = originalLength * material->coefficients[3];

            if(length(distance) > ruptureDistance && !internal) // First test, easyest calculation to return value, probably most of the cases.
                return (double4)0;

            // Elastic regime distance limit:
            double elasticLimitDistance = originalLength * material->coefficients[1];

            if(length(distance) < elasticLimitDistance || internal) {
                // Elastic regime: F = -k*x
                double ea = material->coefficients[0] * contactArea;
                double k = ea / originalLength;

                return -k * distance;
            }

            else {
                double4 distanceUnitary = vector_getUnitary(distance);
                double4 oldForceProjection = dot(oldForce, distanceUnitary) * distanceUnitary;

                // Plastic regime: F = min(plasticMax, oldForce)
                double plasticMaximumForce = material->coefficients[2] * contactArea;

                if(length(oldForceProjection) > plasticMaximumForce)
                    return plasticMaximumForce * vector_getUnitary(oldForceProjection);

                else
                    return -oldForceProjection;
            }
        }
    }

    return (double4)0;
}

double4 material_calculateDragForce(const Material* material, double4 velocity, double4 distance)
{
    switch(material->dragForceType)
    {
        case linear:
            return -(material->dragCoefficients[0]) * velocity;

        case quadratic:
            return -(material->dragCoefficients[0]) * pown(length(velocity), 2) * vector_getUnitary(velocity);

        case cubic:
            return -(material->dragCoefficients[0]) * pown(length(velocity), 3) * vector_getUnitary(velocity);
    }

    return (double4)0;
}

#endif // MATERIAL_CL

//-------------------------------------------------------------------------------------------------------------------------------------------//

#ifndef MATERIALSMANAGER_CL
#define MATERIALSMANAGER_CL

typedef struct
{
    Material materials[20];
    int size;
} MaterialsManager;

const Material* materialsManager_getMaterial(int materialIndex1, int materialIndex2, const MaterialsManager* materialsManager)
{
    if(materialIndex1 == materialIndex2)
        return &(materialsManager->materials[materialIndex1]);

    for(int i = 0; i < materialsManager->size; i++) {
        const Material* material = &(materialsManager->materials[i]);

        if((material->materialIndex1 == materialIndex1 && material->materialIndex2 == materialIndex2) ||
           (material->materialIndex1 == materialIndex2 && material->materialIndex2 == materialIndex1)) {
            return material;
        }
    }

    return 0;
}

#endif // MATERIALSMANAGER_CL

//-------------------------------------------------------------------------------------------------------------------------------------------//

#ifndef PARTICLEWORKER_CL
#define PARTICLEWORKER_CL

void particleToParticleWorker_run(Particle* thisParticle, const Particle* otherParticle, const Material* material)
{
    double4 closestOnThisParticle, closestOnOtherParticle;

    particle_getClosestTo(thisParticle, otherParticle, &closestOnThisParticle, &closestOnOtherParticle);

    double4 distance = closestOnOtherParticle - closestOnThisParticle;

    bool internal = particle_isInternal(thisParticle, closestOnOtherParticle);

    if((length(distance) > material->distanceThreshold) && !internal)
        return;

    double4 velocity = otherParticle->vertex.currentVelocity - thisParticle->vertex.currentVelocity;

    double minAreaThis  = thisParticle->area  / 2.0;
    double minAreaOther = otherParticle->area / 2.0;

    double  minContactArea = min(minAreaThis, minAreaOther);
    double  originalLength = thisParticle->radius + otherParticle->radius;
    double4 oldForce       = otherParticle->oldForce - thisParticle->oldForce;

    double4 force     = material_calculateForce(material, distance, internal, minContactArea, originalLength, oldForce);
    double4 dragForce = material_calculateDragForce(material, velocity, distance);

    double4 totalForce = -(force + dragForce);

    particle_addCurrentForce(thisParticle, &totalForce, &closestOnThisParticle);
}

void particleToFaceWorker_run(Particle* thisParticle, Face* otherFace, const Material* material)
{
    face_calculateCurrentPosition(otherFace);
    face_calculateCurrentVelocity(otherFace);

    double4 closestOnThisParticle, closestOnOtherFace;

    face_getClosestTo(otherFace, thisParticle, &closestOnOtherFace, &closestOnThisParticle);

    double4 distance = closestOnOtherFace - closestOnThisParticle;

    bool internal = particle_isInternal(thisParticle, closestOnOtherFace);

    if((length(distance) > material->distanceThreshold) && !internal)
        return;

    double4 velocity = otherFace->currentVelocity - thisParticle->vertex.currentVelocity;

    double minAreaThis  = thisParticle->area / 2.0;
    double minAreaOther = otherFace->area;

    double  minContactArea = min(minAreaThis, minAreaOther);
    double  originalLength = thisParticle->radius;
    double4 oldForce       = otherFace->oldForce - thisParticle->oldForce;

    double4 force     = material_calculateForce(material, distance, internal, minContactArea, originalLength, oldForce);
    double4 dragForce = material_calculateDragForce(material, velocity, distance);

    double4 totalForce = -(force + dragForce);

    particle_addCurrentForce(thisParticle, &totalForce, &closestOnThisParticle);
}

#endif // PARTICLEWORKER_CL

//-------------------------------------------------------------------------------------------------------------------------------------------//

#ifndef FACEWORKER_CL
#define FACEWORKER_CL

void faceToParticleWorker_run(Face* thisFace, Particle* otherParticle, const Material* material)
{
    face_calculateCurrentPosition(thisFace);
    face_calculateCurrentVelocity(thisFace);

    double4 closestOnThisFace, closestOnOtherParticle;

    face_getClosestTo(thisFace, otherParticle, &closestOnThisFace, &closestOnOtherParticle);

    double4 distance = closestOnOtherParticle - closestOnThisFace;

    bool internal = particle_isInternal(otherParticle, closestOnThisFace);

    if((length(distance) > material->distanceThreshold) && !internal)
        return;

    double4 velocity = otherParticle->vertex.currentVelocity - thisFace->currentVelocity;

    double minAreaThis  = thisFace->area;
    double minAreaOther = otherParticle->area / 2.0;

    double  minContactArea = min(minAreaThis, minAreaOther);
    double  originalLength = otherParticle->radius;
    double4 oldForce       = otherParticle->oldForce - thisFace->oldForce;

    double4 force     = material_calculateForce(material, distance, internal, minContactArea, originalLength, oldForce);
    double4 dragForce = material_calculateDragForce(material, velocity, distance);

    double4 totalForce = -(force + dragForce);

    face_addCurrentForce(thisFace, &totalForce, &closestOnThisFace);
}

#endif // FACEWORKER_CL

//-------------------------------------------------------------------------------------------------------------------------------------------//

#ifndef SCENERY_CL
#define SCENERY_CL

typedef struct
{
    ulong numParticles;
    ulong numFaces;

    double4 gravity;
} Scenery;

#endif // SCENERY_CL

//-------------------------------------------------------------------------------------------------------------------------------------------//

#ifndef SIMULATION_CL
#define SIMULATION_CL

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

    Particle thisParticle = particles[idx];
    MaterialsManager materialsManager = ptrMaterialsManager[0];
    Scenery scenery = ptrScenery[0];

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

kernel void calculate_particle_to_face(global Particle* particles, constant Face* faces, constant MaterialsManager* ptrMaterialsManager, constant Scenery* ptrScenery)
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

kernel void calculate_face_to_particle(global Face* faces, constant Particle* particles, constant MaterialsManager* ptrMaterialsManager, constant Scenery* ptrScenery)
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
