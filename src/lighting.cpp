#include "lighting.hpp"
#include <algorithm>

Color calculateLighting(Color base, Vec3 normal, const DirectionalLight &light,
                        float attenuation) {
  normal = normalize(normal);
  Vec3 lightDir = normalize(light.direction * -1.0f);

  float diffuse = std::max(0.0f, dot(normal, lightDir));

  float intensity = light.ambient + light.diffuse * diffuse * attenuation;
  intensity = std::min(1.0f, intensity);

  return {(unsigned char)(base.r * intensity),
          (unsigned char)(base.g * intensity),
          (unsigned char)(base.b * intensity), base.a};
}
