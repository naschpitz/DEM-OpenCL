#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include "Face.h"
#include "Particle.h"
#include "Vertex.h"

#include <vector>

inline VertexCL makeVertexCL(float x, float y, float z)
{
  VertexCL v{};
  v.currentPosition = {x, y, z, 0.0f};
  return v;
}

inline ParticleCL makeParticleCL(float x, float y, float z, float radius)
{
  ParticleCL p{};
  p.vertex.currentPosition = {x, y, z, 0.0f};
  p.radius = radius;
  return p;
}

inline FaceCL makeFaceCL(const cl_float4& v0, const cl_float4& v1, const cl_float4& v2)
{
  FaceCL f{};
  f.vertexes[0].currentPosition = v0;
  f.vertexes[1].currentPosition = v1;
  f.vertexes[2].currentPosition = v2;
  return f;
}

inline std::vector<FaceCL> makeUnitCubeCL()
{
  cl_float4 c000 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 c100 = {1.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 c010 = {0.0f, 1.0f, 0.0f, 0.0f};
  cl_float4 c110 = {1.0f, 1.0f, 0.0f, 0.0f};
  cl_float4 c001 = {0.0f, 0.0f, 1.0f, 0.0f};
  cl_float4 c101 = {1.0f, 0.0f, 1.0f, 0.0f};
  cl_float4 c011 = {0.0f, 1.0f, 1.0f, 0.0f};
  cl_float4 c111 = {1.0f, 1.0f, 1.0f, 0.0f};

  return {makeFaceCL(c001, c101, c111), makeFaceCL(c001, c111, c011), makeFaceCL(c000, c110, c100),
          makeFaceCL(c000, c010, c110), makeFaceCL(c100, c110, c111), makeFaceCL(c100, c111, c101),
          makeFaceCL(c000, c011, c010), makeFaceCL(c000, c001, c011), makeFaceCL(c010, c110, c111),
          makeFaceCL(c010, c111, c011), makeFaceCL(c000, c101, c100), makeFaceCL(c000, c001, c101)};
}

inline std::vector<FaceCL> makeLShapeCL()
{
  cl_float4 _000 = {0.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 _100 = {1.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 _200 = {2.0f, 0.0f, 0.0f, 0.0f};
  cl_float4 _010 = {0.0f, 1.0f, 0.0f, 0.0f};
  cl_float4 _110 = {1.0f, 1.0f, 0.0f, 0.0f};
  cl_float4 _210 = {2.0f, 1.0f, 0.0f, 0.0f};
  cl_float4 _020 = {0.0f, 2.0f, 0.0f, 0.0f};
  cl_float4 _120 = {1.0f, 2.0f, 0.0f, 0.0f};
  cl_float4 _001 = {0.0f, 0.0f, 1.0f, 0.0f};
  cl_float4 _101 = {1.0f, 0.0f, 1.0f, 0.0f};
  cl_float4 _201 = {2.0f, 0.0f, 1.0f, 0.0f};
  cl_float4 _011 = {0.0f, 1.0f, 1.0f, 0.0f};
  cl_float4 _111 = {1.0f, 1.0f, 1.0f, 0.0f};
  cl_float4 _211 = {2.0f, 1.0f, 1.0f, 0.0f};
  cl_float4 _021 = {0.0f, 2.0f, 1.0f, 0.0f};
  cl_float4 _121 = {1.0f, 2.0f, 1.0f, 0.0f};

  std::vector<FaceCL> faces;

  auto addRect = [&](const cl_float4& a, const cl_float4& b, const cl_float4& c, const cl_float4& d) {
    faces.push_back(makeFaceCL(a, b, c));
    faces.push_back(makeFaceCL(a, c, d));
  };

  addRect(_001, _201, _211, _011);
  addRect(_011, _111, _121, _021);
  addRect(_000, _010, _210, _200);
  addRect(_010, _020, _120, _110);
  addRect(_000, _001, _021, _020);
  addRect(_000, _200, _201, _001);
  addRect(_200, _210, _211, _201);
  addRect(_020, _021, _121, _120);
  addRect(_110, _111, _121, _120);
  addRect(_110, _210, _211, _111);

  return faces;
}

inline void translateFaces(std::vector<FaceCL>& faces, float dx, float dy, float dz)
{
  for (auto& face : faces) {
    for (int i = 0; i < 3; i++) {
      face.vertexes[i].currentPosition.x += dx;
      face.vertexes[i].currentPosition.y += dy;
      face.vertexes[i].currentPosition.z += dz;
    }
  }
}

#endif // TESTHELPERS_H
