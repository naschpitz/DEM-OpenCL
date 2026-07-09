#ifndef TESTSSMOKE_CPP_CL
#define TESTSSMOKE_CPP_CL

kernel void test_smoke_passthrough(global float4* input, global float4* output)
{
  size_t idx = get_global_id(0);

  output[idx] = input[idx];
}

#endif // TESTSSMOKE_CPP_CL
