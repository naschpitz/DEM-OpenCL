#ifndef TESTBOX_CL
#define TESTBOX_CL

#include "../Face.cl"
#include "../Particle.cl"

bool testBoxCommon1(const double4* minThis, const double4* maxThis, const double4* minOther, const double4* maxOther, double distanceThreshold)
{
    double distance;

    // X axis.
    distance = minOther->x - maxThis->x;
    if(distance < 0) distance = minThis->x - maxOther->x;
    if (distance > distanceThreshold) return false;

    // Y axis.
    distance = minOther->y - maxThis->y;
    if(distance < 0) distance = minThis->y - maxOther->y;
    if(distance > distanceThreshold) return false;

    // Z axis.
    distance = minOther->z - maxThis->z;
    if(distance < 0) distance = minThis->z - maxOther->z;
    if(distance > distanceThreshold) return false;

    return true;
}

bool testBoxCommon2(const double4* positionThis, const double4* minOther, const double4* maxOther, double distanceThreshold)
{
    double distance;

    // X axis.
    distance = minOther->x - positionThis->x;
    if(distance < 0) distance = positionThis->x - maxOther->x;
    if(distance > distanceThreshold) return false;

    // Y axis.
    distance = minOther->y - positionThis->y;
    if(distance < 0) distance = positionThis->y - maxOther->y;
    if(distance > distanceThreshold) return false;

    // Z axis.
    distance = minOther->z - positionThis->z;
    if(distance < 0) distance = positionThis->z - maxOther->z;
    if(distance > distanceThreshold) return false;

    return true;
}

bool testBoxCommon3(const double4* positionThis, const double4* positionOther, double distanceThreshold)
{
    double distance;

    // X axis.
    distance = fabs(positionOther->x - positionThis->x);
    if(distance > distanceThreshold) return false;

    // Y axis.
    distance = fabs(positionOther->y - positionThis->y);
    if(distance > distanceThreshold) return false;

    // Z axis.
    distance = fabs(positionOther->z - positionThis->z);
    if(distance > distanceThreshold) return false;

    return true;
}

bool testBox_particleToParticle(const Particle* particleThis, const Particle* particleOther, double distanceThreshold)
{
    const double4* positionThis = &(particleThis->vertex.currentPosition);
    const double4* positionOther = &(particleOther->vertex.currentPosition);

    return testBoxCommon3(positionThis, positionOther, distanceThreshold + particleThis->radius + particleOther->radius);
}

bool testBox_particleToFace(const Particle* particleThis, const Face* faceOther, double distanceThreshold)
{
    const double4* positionThis = &(particleThis->vertex.currentPosition);

    double4 minOther, maxOther;
    face_getBox(faceOther, &minOther, &maxOther);

    return testBoxCommon2(positionThis, &minOther, &maxOther, distanceThreshold + particleThis->radius);
}

#endif // TESTBOX_CL
