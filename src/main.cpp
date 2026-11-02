#include "math3d.hpp"
#include "mesh.hpp"
#include "player.hpp"
#include "primitives.hpp"
#include "renderer.hpp"

#include <GLFW/glfw3.h>
#include <cstdio>
#include <vector>

constexpr int SCREENWIDTH = 960;
constexpr int SCREENHEIGHT = 720;

int main() {
  if (!glfwInit()) {
    std::fprintf(stderr, "Failed to initialize GLFW\n");
    return 1;
  }

  // make it not resizable so it doesnt auto tile on my wm
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  GLFWwindow *window =
      glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "window", nullptr, nullptr);
  if (!window) {
    std::fprintf(stderr, "Failed to create GLFW window\n");
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // vsync

  MouseDelta mouse;
  glfwSetWindowUserPointer(window, &mouse);
  glfwSetCursorPosCallback(window, cursorCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  Player player;
  Renderer renderer(SCREENWIDTH, SCREENHEIGHT);

  // lighting
  renderer.light.direction = {0, -1, 0};
  renderer.light.ambient = 0.2f;
  renderer.light.diffuse = 0.8f;

  std::vector<Mesh> scene = {
      makeRect({0, 200, -20}, {40, 40, 40}),
      makePyramid({60, 200, -20}, {40, 40}),
      makeRect({120, 200, -20}, {40, 40, 40}),
      makeSphere({200, 220, 0}, 64, 20),
  };

  // Testing out color applications
  scene[0].color = RED;
  scene[1].color = BLUE;
  scene[2].color = GREEN;
  scene[3].color = ORANGE;

  double lastTime = glfwGetTime();
  double titleTimer = 0.0;
  int frames = 0;
  int fps = 0;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    double now = glfwGetTime();
    float dt = (float)(now - lastTime);
    lastTime = now;

    player.update(window, mouse, dt);

    float yaw = player.angle * DEG2RAD;
    float pitch = player.pitch * DEG2RAD;

    renderer.light.direction =
        normalize({std::sin(yaw) * std::cos(pitch),
                   std::cos(yaw) * std::cos(pitch), std::sin(pitch)});

    renderer.clear(RAYWHITE);
    renderer.drawMeshes(scene, player);
    renderer.present(window);

    glfwSwapBuffers(window);

    frames++;
    titleTimer += dt;
    if (titleTimer >= 0.5) {
      fps = (int)(frames / titleTimer);
      frames = 0;
      titleTimer = 0.0;

      // Show some info in title bar but
      // i cant see cuz i use mangowm fml windows > linux
      char title[256];
      std::snprintf(
          title, sizeof title,
          "window | POS %.0f, %.0f, %.0f | ANGLE %.0f | PITCH %.0f | FPS %d",
          player.pos.x, player.pos.y, player.pos.z, player.angle, player.pitch,
          fps);
      glfwSetWindowTitle(window, title);
    }
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
