#include "primitives.hpp"
#include "math3d.hpp"
#include "mesh.hpp"

/* daddy yamato pls refactor on nov3. */

Mesh makeRect(Vec3 origin, Vec3 size, std::string name) {
  Mesh m;
  float x = origin.x, y = origin.y, z = origin.z;
  float w = size.x, d = size.y, h = size.z;

  m.name = name;

  // 8 corners
  m.vertices = {
      {x, y, z},
      {x + w, y, z},
      {x + w, y + d, z},
      {x, y + d, z},
      {x, y, z + h},
      {x + w, y, z + h},
      {x + w, y + d, z + h},
      {x, y + d, z + h},
  };

  m.triangles = {
      {0, 2, 1}, {0, 3, 2}, // bottom
      {4, 5, 6}, {4, 6, 7}, // top
      {0, 1, 5}, {0, 5, 4}, // front
      {2, 3, 7}, {2, 7, 6}, // back
      {1, 2, 6}, {1, 6, 5}, // right
      {3, 0, 4}, {3, 4, 7}, // left
  };
  return m;
}

Mesh makePyramid(Vec3 origin, Vec2 dim, std::string name) {
  Mesh m;
  float x = origin.x, y = origin.y, z = origin.z;
  float b = dim.x, h = dim.y;

  m.name = name;

  // 5 corners
  m.vertices = {
      {x, y, z},
      {x + b, y, z},
      {x + b, y + b, z},
      {x, y + b, z},
      {x + b / 2, y + b / 2, z + h},
  };

  m.triangles = {
      {0, 2, 1}, {0, 3, 2}, {0, 1, 4}, {1, 2, 4}, {2, 3, 4}, {3, 0, 4},
  };

  return m;
}

Mesh makeSphere(Vec3 origin, int seg, float r, std::string name) {
  Mesh m;

  m.name = name;

  for (int i = 0; i <= seg; i++) {
    float theta = i * PI / seg;

    for (int j = 0; j <= seg; j++) {
      float phi = j * 2 * PI / seg;

      float x = r * sinf(theta) * cosf(phi);
      float y = r * sinf(theta) * sinf(phi);
      float z = r * cosf(theta);

      m.vertices.push_back({origin.x + x, origin.y + y, origin.z + z});
    }
  }

  for (int i = 0; i < seg; i++) {
    for (int j = 0; j < seg; j++) {
      int a = i * (seg + 1) + j;
      int b = a + 1;
      int c = (i + 1) * (seg + 1) + j;
      int d = c + 1;

      m.triangles.push_back({a, c, b});
      m.triangles.push_back({b, c, d});
    }
  }

  return m;
}

// donut
Mesh makeTorus(float R, float r, int seg, int ring, std::string name) {
  Mesh m;

  m.name = name;

  for (int i = 0; i < seg; i++) {
    float u = i * 2 * PI / seg;
    for (int j = 0; j < ring; j++) {
      float v = j * 2 * PI / ring;
      m.vertices.push_back({(R + r * cosf(v)) * cosf(u),
                            (R + r * cosf(v)) * sinf(u), r * sinf(v)});
    }
  }

  for (int i = 0; i < seg; i++)
    for (int j = 0; j < ring; j++) {
      int a = i * ring + j;
      int b = ((i + 1) % seg) * ring + j;
      int c = ((i + 1) % seg) * ring + (j + 1) % ring;
      int d = i * ring + (j + 1) % ring;
      m.triangles.push_back({a, b, c});
      m.triangles.push_back({a, c, d});
    }
  return m;
}
