#ifndef FACE_CL
#define FACE_CL

#include "../Edge.cl"
#include "../Particle.cl"
#include "../Vector.cl"
#include "../Vertex.cl"

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
