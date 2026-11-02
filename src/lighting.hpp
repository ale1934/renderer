#pragma once

#include "math3d.hpp"

struct DirectionalLight {
  Vec3 direction = {-1, -1, -1};
  float ambient = 0.2f;
  float diffuse = 0.8f;
};

Color calculateLighting(Color base, Vec3 normal, const DirectionalLight &light,
                        float attenuation);
