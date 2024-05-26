#ifndef FACE_CPP_CL
#define FACE_CPP_CL

#include "../Face.h.cl"
#include "../Particle.h.cl"

#include "../Edge.cpp.cl"
#include "../Utils.cpp.cl"
#include "../Vector.cpp.cl"
#include "../Vertex.cpp.cl"

void face_addCurrentForce(Face* face, const float4* force, const float4* pointOfForce)
{
    face->currentForce += (*force);

    float4 r = (*pointOfForce) - face->currentPosition;
    face->currentTorque += cross(r, (*force));
}

void face_atomicAddCurrentForce(volatile global Face* face, const float4* force, const float4* pointOfForce)
{
    atomic_add_float(&(face->currentForceX), force->x);
    atomic_add_float(&(face->currentForceY), force->y);
    atomic_add_float(&(face->currentForceZ), force->z);

    float4 r = (*pointOfForce) - face->currentPosition;
    float4 torque = cross(r, (*force));

    atomic_add_float(&(face->currentTorqueX), torque.x);
    atomic_add_float(&(face->currentTorqueY), torque.y);
    atomic_add_float(&(face->currentTorqueZ), torque.z);
}

void face_getBox(const Face* face, float4* min, float4* max)
{
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;

    minX = maxX = face->vertexes[0].currentPosition.x;
    minY = maxY = face->vertexes[0].currentPosition.y;
    minZ = maxZ = face->vertexes[0].currentPosition.z;

    for(int i = 0; i < 3; i++) {
        const float4* position = &(face->vertexes[i].currentPosition);

        if(position->x < minX) minX = position->x;
        if(position->x > maxX) maxX = position->x;

        if(position->y < minY) minY = position->y;
        if(position->y > maxY) maxY = position->y;

        if(position->z < minZ) minZ = position->z;
        if(position->z > maxZ) maxZ = position->z;
    }

    min->x = minX;
    min->y = minY;
    min->z = minZ;

    max->x = maxX;
    max->y = maxY;
    max->z = maxZ;
}

void face_getClosestTo(volatile global const Face* thisFace, volatile global const Particle* otherParticle, float4* closestOnThisFace, float4* closestOnOtherParticle)
{
    float4 p1 = thisFace->vertexes[0].currentPosition;
    float4 p2 = thisFace->vertexes[1].currentPosition;
    float4 p3 = thisFace->vertexes[2].currentPosition;

    float4 q = otherParticle->vertex.currentPosition;

    float4 a =  q - p1;
    float4 b = p2 - p1;
    float4 c = p3 - p1;

    float ab = dot(a, b);
    float ac = dot(a, c);
    float bb = dot(b, b);
    float bc = dot(b, c);
    float cc = dot(c, c);

    float  den = bb * cc - bc * bc;
    float numU = ab * cc - ac * bc;
    float numV = ac * bb - ab * bc;

    float u = numU / den;
    float v = numV / den;

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

    float4 distance = otherParticle->vertex.currentPosition - (*closestOnThisFace);
    float4 distanceUnitary = vector_getUnitary(distance);

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

float4 face_getCurrentAcceleration(Face* face)
{
    return face->currentForce / face->mass;
}

void face_integrate(Face* face, float timeStep)
{
    face->currentForce = (float4)(face->currentForce.x, face->currentForce.y, face->currentForce.z, 0);
    face->currentTorque = (float4)(face->currentTorque.x, face->currentTorque.y, face->currentTorque.z, 0);

    float4 currentAcceleration = face_getCurrentAcceleration(face);

    for(int i = 0; i < 3; i++) {
        face->vertexes[i].acceleration = currentAcceleration;
        vertex_integrate(face->vertexes + i, timeStep);
    }

    face->oldForce = face->currentForce;
    face->oldTorque = face->currentTorque;

    face_calculateCurrentPosition(face);
    face_calculateCurrentVelocity(face);
}

#endif // FACE_CPP_CL
