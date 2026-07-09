#ifndef FACEWORKER_CPP_CL
#define FACEWORKER_CPP_CL

#include "../opencl/Face.cpp.cl"
#include "../opencl/Particle.cpp.cl"
#include "../opencl/Material.cpp.cl"
#include "../opencl/TestBox.cpp.cl"

void faceToParticleWorker_run(Face* thisFace, Particle* otherParticle, const Material* material)
{
  float4 closestOnThisFace, closestOnOtherParticle;

  face_getClosestToParticle(thisFace, otherParticle, &closestOnThisFace, &closestOnOtherParticle);

  float4 distance = closestOnOtherParticle - closestOnThisFace;
  float4 distanceUnitary = vector_getUnitary(distance);

  bool internal = particle_isInternal(otherParticle, closestOnThisFace);

  if ((length(distance) > material->distanceThreshold) && !internal)
    return;

  float4 velocity = otherParticle->vertex.currentVelocity - thisFace->currentVelocity;

  float4 otherR = closestOnOtherParticle - otherParticle->vertex.currentPosition;
  float4 rotationVelocity = cross(otherParticle->vertex.currentAngularVelocity, otherR);

  float minAreaThis = thisFace->area;
  float minAreaOther = otherParticle->area / 2.0;

  float minContactArea = min(minAreaThis, minAreaOther);
  float originalLength = otherParticle->radius;
  float4 oldForce = otherParticle->oldForce - thisFace->oldForce;

  float4 force =
    material_calculateForce(material, distance, distanceUnitary, internal, minContactArea, originalLength, oldForce);
  float4 dragForce = material_calculateDragForce(material, velocity, rotationVelocity, force);

  float4 totalForce = -(force + dragForce);

  uint numFaces = otherParticle->neighborhood.numFaces;

  thisFace->currentForce += totalForce / numFaces;

  float4 r = closestOnThisFace - thisFace->currentPosition;
  float4 torque = cross(r, totalForce) / numFaces;

  thisFace->currentTorque += torque;
}

void faceToFaceWorker_run(Face* thisFace, const Face* otherFace, const Material* material)
{
  float4 closestOnThisFace, closestOnOtherFace;

  face_getClosestToFace(thisFace, otherFace, &closestOnThisFace, &closestOnOtherFace);

  float4 distance = closestOnOtherFace - closestOnThisFace;
  float4 distanceUnitary = vector_getUnitary(distance);

  if (length(distance) > material->distanceThreshold)
    return;

  float4 velocity = otherFace->currentVelocity - thisFace->currentVelocity;

  float minContactArea = min(thisFace->area, otherFace->area);
  float originalLength = 0.0f;
  float4 oldForce = otherFace->oldForce - thisFace->oldForce;

  float4 force =
    material_calculateForce(material, distance, distanceUnitary, false, minContactArea, originalLength, oldForce);
  float4 dragForce = material_calculateDragForce(material, velocity, (float4)0, force);

  float4 totalForce = -(force + dragForce);

  thisFace->currentForce += totalForce;

  float4 r = closestOnThisFace - thisFace->currentPosition;
  float4 torque = cross(r, totalForce);

  thisFace->currentTorque += torque;
}

#endif // FACEWORKER_CPP_CL
