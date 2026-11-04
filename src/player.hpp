#pragma once

#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "math3d.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>

struct MouseDelta {
  double lastX = 0, lastY = 0;
  bool hasLast = false;
  double dx = 0, dy = 0;
};

static void cursorCallback(GLFWwindow *window, double x, double y) {
  ImGui_ImplGlfw_CursorPosCallback(window, x, y);

  auto *mouse = static_cast<MouseDelta *>(glfwGetWindowUserPointer(window));
  if (mouse->hasLast) {
    mouse->dx += x - mouse->lastX;
    mouse->dy += y - mouse->lastY;
  }
  mouse->lastX = x;
  mouse->lastY = y;
  mouse->hasLast = true;
}

static bool isDown(GLFWwindow *window, int key) {
  return glfwGetKey(window, key) == GLFW_PRESS;
}

class Player {
public:
  Vec3 pos = {70, 0, 0};
  float angle = 0;
  float pitch = 0;

  int mouseWarmupFrames = 10;

  void update(GLFWwindow *window, MouseDelta &mouse, float dt);
};
