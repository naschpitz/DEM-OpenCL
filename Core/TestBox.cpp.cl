#ifndef TESTBOX_CPP_CL
#define TESTBOX_CPP_CL

bool testBoxCommon1(const float4* minThis, const float4* maxThis, const float4* minOther, const float4* maxOther, float distanceThreshold)
{
    float distance;

    // X axis.
    distance = minOther->x - maxThis->x;
    if(distance < 0) distance = minThis->x - maxOther->x;
    if(distance > distanceThreshold) return false;

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

bool testBoxCommon2(const float4* positionThis, const float4* minOther, const float4* maxOther, float distanceThreshold)
{
    float distance;

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

bool testBoxCommon3(const float4* positionThis, const float4* positionOther, float distanceThreshold)
{
    float distance;

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

bool testBox_particleToParticle(const Particle* particleThis, const Particle* particleOther, float distanceThreshold)
{
    const float4* positionThis = &(particleThis->vertex.currentPosition);
    const float4* positionOther = &(particleOther->vertex.currentPosition);

    return testBoxCommon3(positionThis, positionOther, distanceThreshold + particleThis->radius + particleOther->radius);
}

bool testBox_particleToFace(const Particle* particleThis, const Face* faceOther, float distanceThreshold)
{
    const float4* positionThis = &(particleThis->vertex.currentPosition);

    float4 minOther, maxOther;
    face_getBox(faceOther, &minOther, &maxOther);

    return testBoxCommon2(positionThis, &minOther, &maxOther, distanceThreshold + particleThis->radius);
}

#endif // TESTBOX_CPP_CL
