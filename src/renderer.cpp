#include "renderer.hpp"
#include "math3d.hpp"
#include <vector>

void Renderer::drawLine(Vec3 pointA, Vec3 pointB, const Player &cam,
                        Color color) {
  // Transform world coordinates into camera space
  Vec3 a = toCameraSpace(pointA, cam);
  Vec3 b = toCameraSpace(pointB, cam);

  // Clip against near plane
  if (a.y < nearPlane && b.y < nearPlane)
    return;

  if (a.y < nearPlane)
    a = intersectNearPlane(a, b);
  else if (b.y < nearPlane)
    b = intersectNearPlane(b, a);

  // Project onto screen
  Vec2 p1 = project(a);
  Vec2 p2 = project(b);

  // DDA line rasterization
  float dx = p2.x - p1.x;
  float dy = p2.y - p1.y;

  int steps = (int)std::ceil(std::max(std::abs(dx), std::abs(dy)));

  if (steps == 0) {
    int x = (int)p1.x;
    int y = (int)p1.y;

    if (x >= 0 && x < width && y >= 0 && y < height)
      framebuffer[y * width + x] = color;

    return;
  }

  float xStep = dx / steps;
  float yStep = dy / steps;

  float x = p1.x;
  float y = p1.y;

  for (int i = 0; i <= steps; i++) {
    int px = (int)std::round(x);
    int py = (int)std::round(y);

    if (px >= 0 && px < width && py >= 0 && py < height)
      framebuffer[py * width + px] = color;

    x += xStep;
    y += yStep;
  }
}

void Renderer::clear(Color c) {
  std::fill(framebuffer.begin(), framebuffer.end(), c);
}

void Renderer::drawMeshes(const std::vector<Mesh> &meshes,
                          const Player &camera) {
  std::vector<DrawFace> faces;
  for (const auto &m : meshes)
    collectMesh(m, camera, faces);

  std::sort(
      faces.begin(), faces.end(),
      [](const DrawFace &a, const DrawFace &b) { return a.depth > b.depth; });

  for (const auto &f : faces)
    drawTriangle(f);
}

// Copy the software framebuffer to the window, scaled to fill it.
void Renderer::present(GLFWwindow *window) {
  int winW = 0, winH = 0;
  glfwGetFramebufferSize(window, &winW, &winH);

  glViewport(0, 0, winW, winH);
  glClearColor(RAYWHITE.r / 255.0f, RAYWHITE.g / 255.0f, RAYWHITE.b / 255.0f,
               1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glRasterPos2f(-1.0f, 1.0f);
  glPixelZoom((float)winW / width, -(float)winH / height);
  glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.data());
}

// Convert world coordinates into camera coordinates.
Vec3 Renderer::toCameraSpace(Vec3 point, const Player &camera) {
  float yaw = camera.angle * DEG2RAD;
  float pitch = camera.pitch * DEG2RAD;

  float CS = std::cos(yaw);
  float SN = std::sin(yaw);
  float CP = std::cos(pitch);
  float SP = std::sin(pitch);

  float x = point.x - camera.pos.x;
  float y = point.y - camera.pos.y;
  float z = point.z - camera.pos.z;

  // Yaw: rotate around the vertical axis.
  float xr = x * CS - y * SN;
  float yr = x * SN + y * CS;

  // Pitch: tilt forward and up around the sideways axis.
  float zr = z * CP - yr * SP;
  float yf = z * SP + yr * CP;

  return {xr, yf, zr};
}

// Find where an edge intersects the near plane.
Vec3 Renderer::intersectNearPlane(Vec3 a, Vec3 b) {
  float t = (nearPlane - a.y) / (b.y - a.y);
  return {a.x + t * (b.x - a.x), nearPlane, a.z + t * (b.z - a.z)};
}

std::vector<Vec3> Renderer::clipPolygon(const std::vector<Vec3> &polygon) {
  std::vector<Vec3> result;
  if (polygon.empty())
    return result;

  for (size_t i = 0; i < polygon.size(); i++) {
    Vec3 current = polygon[i];
    Vec3 next = polygon[(i + 1) % polygon.size()];

    bool currentInside = current.y >= nearPlane;
    bool nextInside = next.y >= nearPlane;

    if (currentInside && nextInside) {
      result.push_back(next);
    } else if (currentInside && !nextInside) {
      result.push_back(intersectNearPlane(current, next));
    } else if (!currentInside && nextInside) {
      result.push_back(intersectNearPlane(current, next));
      result.push_back(next);
    }
  }
  return result;
}

void Renderer::collectMesh(const Mesh &mesh, const Player &camera,
                           std::vector<DrawFace> &out) {
  // Transform each vertex to world space, then to camera space, once.
  std::vector<Vec3> world(mesh.vertices.size());
  std::vector<Vec3> cam(mesh.vertices.size());
  for (size_t i = 0; i < mesh.vertices.size(); i++) {
    world[i] = mesh.vertices[i] + mesh.position;
    cam[i] = toCameraSpace(world[i], camera);
  }

  for (const auto &t : mesh.triangles) {
    constexpr float falloffDistance = 500.0f;

    Vec3 A = world[t.a], B = world[t.b], C = world[t.c];
    Vec3 center = (A + B + C) * (1.0f / 3.0f);
    Vec3 toCamera = center - camera.pos;
    Vec3 normal = cross(B - A, C - A);
    float distance = std::sqrt(dot(toCamera, toCamera));
    float attenuation = 1.0f / (1.0f + (distance * distance) /
                                           (falloffDistance * falloffDistance));

    if (dot(normal, camera.pos - center) <= 0)
      continue;

    float depth = (cam[t.a].y + cam[t.b].y + cam[t.c].y) / 3.0f;

    Color shaded = calculateLighting(mesh.color, normal, light, attenuation);

    out.push_back({{cam[t.a], cam[t.b], cam[t.c]}, depth, shaded});
  }
}

void Renderer::drawTriangle(const DrawFace &f) {
  std::vector<Vec3> polygon = clipPolygon({f.verts[0], f.verts[1], f.verts[2]});
  if (polygon.size() < 3)
    return;

  std::vector<Vec2> projected;
  projected.reserve(polygon.size());
  for (const Vec3 &v : polygon)
    projected.push_back(project(v));

  for (size_t i = 1; i + 1 < projected.size(); i++)
    fillTriangle(projected[0], projected[i], projected[i + 1], f.color);
}

// Project a camera-space point onto the screen.
Vec2 Renderer::project(Vec3 point) {
  return {point.x * focalLength / point.y + width / 2.0f,
          -point.z * focalLength / point.y + height / 2.0f};
}

void Renderer::fillTriangle(Vec2 a, Vec2 b, Vec2 c, Color col) {
  if (!std::isfinite(a.x) || !std::isfinite(a.y) || !std::isfinite(b.x) ||
      !std::isfinite(b.y) || !std::isfinite(c.x) || !std::isfinite(c.y))
    return;

  // Sort vertices by y.
  if (a.y > b.y)
    std::swap(a, b);
  if (a.y > c.y)
    std::swap(a, c);
  if (b.y > c.y)
    std::swap(b, c);

  if (c.y - a.y < 1e-6f)
    return;

  int yStart = std::max(0, (int)std::ceil(a.y));
  int yEnd = std::min(height - 1, (int)std::floor(c.y));

  for (int y = yStart; y <= yEnd; y++) {
    float t = (y - a.y) / (c.y - a.y);
    float xl = a.x + (c.x - a.x) * t;

    float xr;
    if (y < b.y) {
      float d = b.y - a.y;
      xr = (d > 1e-6f) ? a.x + (b.x - a.x) * ((y - a.y) / d) : b.x;
    } else {
      float d = c.y - b.y;
      xr = (d > 1e-6f) ? b.x + (c.x - b.x) * ((y - b.y) / d) : c.x;
    }

    float x0 = std::min(xl, xr);
    float x1 = std::max(xl, xr);

    int xs = std::max(0, (int)std::ceil(x0));
    int xe = std::min(width - 1, (int)std::floor(x1));

    Color *row = &framebuffer[static_cast<size_t>(y) * width];
    for (int x = xs; x <= xe; x++)
      row[x] = col;
  }
}

void Renderer::showCoords(const Player &cam) {
  drawLine({0.0f, 0.0f, 0.0f}, {1000.0f, 0.0f, 0.0f}, cam, RED);
  drawLine({0.0f, 0.0f, 0.0f}, {0.0f, 1000.0f, 0.0f}, cam, GREEN);
  drawLine({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1000.0f}, cam, BLUE);
}
