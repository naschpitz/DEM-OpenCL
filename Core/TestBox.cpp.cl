#ifndef TESTBOX_CPP_CL
#define TESTBOX_CPP_CL

bool testBoxCommon1(const float4* thisMin, const float4* thisMax, const float4* otherMin, const float4* otherMax, float distanceThreshold)
{
    float distance;

    // X axis.
    distance = otherMin->x - thisMax->x;
    if(distance < 0) distance = thisMin->x - otherMax->x;
    if(distance > distanceThreshold) return false;

    // Y axis.
    distance = otherMin->y - thisMax->y;
    if(distance < 0) distance = thisMin->y - otherMax->y;
    if(distance > distanceThreshold) return false;

    // Z axis.
    distance = otherMin->z - thisMax->z;
    if(distance < 0) distance = thisMin->z - otherMax->z;
    if(distance > distanceThreshold) return false;

    return true;
}

bool testBoxCommon2(const float4* thisPosition, const float4* otherMin, const float4* otherMax, float distanceThreshold)
{
    float distance;

    // X axis.
    distance = otherMin->x - thisPosition->x;
    if(distance < 0) distance = thisPosition->x - otherMax->x;
    if(distance > distanceThreshold) return false;

    // Y axis.
    distance = otherMin->y - thisPosition->y;
    if(distance < 0) distance = thisPosition->y - otherMax->y;
    if(distance > distanceThreshold) return false;

    // Z axis.
    distance = otherMin->z - thisPosition->z;
    if(distance < 0) distance = thisPosition->z - otherMax->z;
    if(distance > distanceThreshold) return false;

    return true;
}

bool testBoxCommon3(const float4* thisPosition, const float4* otherPosition, float distanceThreshold)
{
    float distance;

    // X axis.
    distance = fabs(otherPosition->x - thisPosition->x);
    if(distance > distanceThreshold) return false;

    // Y axis.
    distance = fabs(otherPosition->y - thisPosition->y);
    if(distance > distanceThreshold) return false;

    // Z axis.
    distance = fabs(otherPosition->z - thisPosition->z);
    if(distance > distanceThreshold) return false;

    return true;
}

bool testBox_particleToParticle(const Particle* thisParticle, const Particle* otherParticle, float distanceThreshold)
{
    const float4* thisPosition = &(thisParticle->vertex.currentPosition);
    const float4* otherPosition = &(otherParticle->vertex.currentPosition);

    return testBoxCommon3(thisPosition, otherPosition, distanceThreshold + thisParticle->radius + otherParticle->radius);
}

bool testBox_particleToFace(const Particle* thisParticle, const Face* otherFace, float distanceThreshold)
{
    const float4* thisPosition = &(thisParticle->vertex.currentPosition);

    float4 otherMin, otherMax;
    face_getBox(otherFace, &otherMin, &otherMax);

    return testBoxCommon2(thisPosition, &otherMin, &otherMax, distanceThreshold + thisParticle->radius);
}

bool testBox_faceToFace(const Face* thisFace, const Face* otherFace, float distanceThreshold)
{
    float4 thisMin, thisMax, otherMin, otherMax;
    face_getBox(thisFace, &thisMin, &thisMax);
    face_getBox(otherFace, &otherMin, &otherMax);

    return testBoxCommon1(&thisMin, &thisMax, &otherMin, &otherMax, distanceThreshold);
}

#endif // TESTBOX_CPP_CL
