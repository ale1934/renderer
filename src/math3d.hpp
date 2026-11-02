#pragma once

#include <cmath>

constexpr float PI = 3.14159265358979323846f;
constexpr float DEG2RAD = PI / 180.0f;

struct Vec2 {
  float x = 0, y = 0;
};

struct Vec3 {
  float x = 0, y = 0, z = 0;
};

inline Vec3 operator+(Vec3 a, Vec3 b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}
inline Vec3 operator-(Vec3 a, Vec3 b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}
inline Vec3 operator*(Vec3 a, float s) { return {a.x * s, a.y * s, a.z * s}; }
inline Vec3 &operator+=(Vec3 &a, Vec3 b) {
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;
  return a;
}

inline float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

inline Vec3 cross(Vec3 a, Vec3 b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

inline Vec3 normalize(Vec3 v) {
  float len = std::sqrt(dot(v, v));

  if (len < 1e-6f)
    return {0, 0, 0};

  return v * (1.0f / len);
}

// RGBA8, laid out so it can be handed straight to glDrawPixels(GL_RGBA, ...).
struct Color {
  unsigned char r, g, b, a;
};

inline constexpr Color RAYWHITE{245, 245, 245, 255};
inline constexpr Color BLACK{0, 0, 0, 255};
inline constexpr Color RED{230, 41, 55, 255};
inline constexpr Color GREEN{0, 228, 48, 255};
inline constexpr Color BLUE{0, 121, 241, 255};
inline constexpr Color ORANGE{255, 161, 0, 255};
inline constexpr Color PURPLE{200, 122, 255, 255};
inline constexpr Color GRAY{130, 130, 130, 255};
