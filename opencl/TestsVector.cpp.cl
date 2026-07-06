#ifndef TESTSVECTOR_CPP_CL
#define TESTSVECTOR_CPP_CL

#include "../opencl/Vector.cpp.cl"

kernel void test_vector_getUnitary(global float4* input, global float4* output)
{
  size_t i = get_global_id(0);

  output[i] = vector_getUnitary(input[i]);
}

#endif // TESTSVECTOR_CPP_CL
