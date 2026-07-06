#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <sstream>
#include <string>

// Builds an ASCII STL surface (12 triangles, 2 per face) for an axis-aligned
// box [minX,maxX] x [minY,maxY] x [minZ,maxZ]. Consumed by SolidObject::loadStl()
// through the JSON "stl" field, so a SolidObject fixture can be authored in C++
// without a hand-written STL blob.
inline std::string makeBoxSTL(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
{
  auto vertex = [](double x, double y, double z) {
    std::ostringstream os;
    os << "vertex " << x << " " << y << " " << z << "\n";
    return os.str();
  };

  auto facet = [&](const std::string& normal, const std::string& a, const std::string& b, const std::string& c) {
    std::ostringstream os;
    os << "facet normal " << normal << "\nouter loop\n" << a << b << c << "endloop\nendfacet\n";
    return os.str();
  };

  std::string c000 = vertex(minX, minY, minZ), c100 = vertex(maxX, minY, minZ);
  std::string c010 = vertex(minX, maxY, minZ), c110 = vertex(maxX, maxY, minZ);
  std::string c001 = vertex(minX, minY, maxZ), c101 = vertex(maxX, minY, maxZ);
  std::string c011 = vertex(minX, maxY, maxZ), c111 = vertex(maxX, maxY, maxZ);

  std::ostringstream stl;
  stl << "solid box\n";
  stl << facet("1 0 0", c100, c101, c111);
  stl << facet("1 0 0", c100, c111, c110);
  stl << facet("-1 0 0", c000, c011, c001);
  stl << facet("-1 0 0", c000, c010, c011);
  stl << facet("0 1 0", c010, c111, c011);
  stl << facet("0 1 0", c010, c110, c111);
  stl << facet("0 -1 0", c000, c101, c100);
  stl << facet("0 -1 0", c000, c001, c101);
  stl << facet("0 0 1", c001, c111, c101);
  stl << facet("0 0 1", c001, c011, c111);
  stl << facet("0 0 -1", c000, c110, c010);
  stl << facet("0 0 -1", c000, c100, c110);
  stl << "endsolid box\n";

  return stl.str();
}

#endif // TESTHELPERS_H
