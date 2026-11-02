#pragma once

#include <GLFW/glfw3.h>
#include <algorithm>
#include <vector>

#include "lighting.hpp"
#include "math3d.hpp"
#include "mesh.hpp"
#include "player.hpp"

class Renderer {
public:
  DirectionalLight light;

  Renderer(int w, int h)
      : width(w), height(h),
        framebuffer(static_cast<size_t>(w) * static_cast<size_t>(h), RAYWHITE) {
  }

  void clear(Color c);

  void drawMeshes(const std::vector<Mesh> &meshes, const Player &camera);

  // Copy the software framebuffer to the window, scaled to fill it.
  void present(GLFWwindow *window);

private:
  int width;
  int height;

  float focalLength = 600.0f;
  float nearPlane = 0.1f;

  std::vector<Color> framebuffer;

  // Convert world coordinates into camera coordinates.
  Vec3 toCameraSpace(Vec3 point, const Player &camera);

  // Find where an edge intersects the near plane.
  Vec3 intersectNearPlane(Vec3 a, Vec3 b);

  // Sutherland-Hodgman polygon clipping against the near plane.
  std::vector<Vec3> clipPolygon(const std::vector<Vec3> &polygon);

  void collectMesh(const Mesh &mesh, const Player &camera,
                   std::vector<DrawFace> &out);

  void drawTriangle(const DrawFace &f);

  // Project a camera-space point onto the screen.
  Vec2 project(Vec3 point);

  void fillTriangle(Vec2 a, Vec2 b, Vec2 c, Color col);
};
