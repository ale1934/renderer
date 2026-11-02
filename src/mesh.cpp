#include "mesh.hpp"

// Add mesh to mesh
void Mesh::append(const Mesh &other) {
  int base = (int)vertices.size();
  for (const auto &v : other.vertices)
    vertices.push_back(v);
  for (auto t : other.triangles) {
    t.a += base;
    t.b += base;
    t.c += base;
    triangles.push_back(t);
  }
}
