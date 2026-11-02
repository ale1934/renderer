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

// Testing hsv colors
Color HSVtoRGB(float h, float s, float v) {
  h = fmodf(h, 360.0f);
  if (h < 0)
    h += 360.0f;

  float c = v * s;
  float x = c * (1 - fabsf(fmodf(h / 60.0f, 2) - 1));
  float m = v - c;

  float r, g, b;

  if (h < 60) {
    r = c;
    g = x;
    b = 0;
  } else if (h < 120) {
    r = x;
    g = c;
    b = 0;
  } else if (h < 180) {
    r = 0;
    g = c;
    b = x;
  } else if (h < 240) {
    r = 0;
    g = x;
    b = c;
  } else if (h < 300) {
    r = x;
    g = 0;
    b = c;
  } else {
    r = c;
    g = 0;
    b = x;
  }

  return {(unsigned char)((r + m) * 255), (unsigned char)((g + m) * 255),
          (unsigned char)((b + m) * 255), 255};
}

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

  std::vector<Mesh> scene = {
      makeRect({0, 200, -20}, {40, 40, 40}),
      makePyramid({60, 200, -20}, {40, 40}),
      makeRect({120, 200, -20}, {40, 40, 40}),
  };

  // Testing out color applications
  scene[0].color = RED;
  scene[1].color = BLUE;
  scene[2].color = GREEN;

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
