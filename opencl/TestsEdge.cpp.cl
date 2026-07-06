#ifndef TESTSEDGE_CPP_CL
#define TESTSEDGE_CPP_CL

#include "../opencl/Edge.cpp.cl"

kernel void test_edge_getClosestTo(global Vertex* edgeVertices, global float4* queryPoints, global float4* output)
{
  size_t i = get_global_id(0);

  Edge localEdge;
  localEdge.v1 = edgeVertices[i * 2];
  localEdge.v2 = edgeVertices[i * 2 + 1];

  float4 query = queryPoints[i];
  float4 closest = (float4)0;

  edge_getClosestTo(&localEdge, &query, &closest);

  output[i] = closest;
}

#endif // TESTSEDGE_CPP_CL
