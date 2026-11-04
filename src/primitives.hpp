#pragma once

#include "math3d.hpp"
#include "mesh.hpp"

Mesh makeRect(Vec3 origin, Vec3 size, std::string name = "rect");
Mesh makePyramid(Vec3 origin, Vec2 dim, std::string name = "pyramid");
Mesh makeSphere(Vec3 origin, int seg, float r, std::string name = "sphere");
Mesh makeTorus(float R, float r, int seg, int ring, std::string name = "torus");
