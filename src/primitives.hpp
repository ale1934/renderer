#pragma once

#include "math3d.hpp"
#include "mesh.hpp"

Mesh makeRect(Vec3 origin, Vec3 size);
Mesh makePyramid(Vec3 origin, Vec2 dim);
Mesh makeSphere(Vec3 origin, int seg, float r);
Mesh makeTorus(float R, float r, int seg, int ring);
