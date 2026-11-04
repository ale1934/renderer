#pragma once

#include "math3d.hpp"

#include <array>
#include <string>
#include <vector>

struct Triangle {
  int a, b, c;
};

struct DrawFace {
  std::array<Vec3, 3> verts; // camera space
  float depth;
  Color color;
};

class Mesh {
public:
  // Default Color :D
  Color color = RAYWHITE;
  std::string name;

  std::vector<Vec3> vertices;
  std::vector<Triangle> triangles;
  Vec3 position = {0, 0, 0};

  void append(const Mesh &other);
};
