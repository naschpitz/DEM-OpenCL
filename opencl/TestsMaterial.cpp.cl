#ifndef TESTSMATERIAL_CPP_CL
#define TESTSMATERIAL_CPP_CL

#include "../opencl/Material.cpp.cl"

kernel void test_material_calculateForce(global const Material* materials, float4 distance, float4 distanceUnitary,
                                         int internal, float contactArea, float originalLength, float4 oldForce,
                                         global float4* output)
{
  output[0] = material_calculateForce(&materials[0], distance, distanceUnitary, (bool)internal, contactArea,
                                      originalLength, oldForce);
}

kernel void test_material_calculateDragForce(global const Material* materials, float4 velocity, float4 rotationVelocity,
                                             float4 force, global float4* output)
{
  output[0] = material_calculateDragForce(&materials[0], velocity, rotationVelocity, force);
}

#endif // TESTSMATERIAL_CPP_CL
