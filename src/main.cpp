#include "math3d.hpp"
#include "mesh.hpp"
#include "player.hpp"
#include "primitives.hpp"
#include "renderer.hpp"

#include <GLFW/glfw3.h>
#include <cstdio>
#include <vector>

// Imgui
#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "../external/imgui/backends/imgui_impl_opengl3.h"
#include <imgui.h>

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

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

  // initialize imgui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  bool show_demo_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

  bool menuToggleReleased = true;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && menuToggleReleased) {
      show_demo_window = !show_demo_window;
      glfwSetInputMode(window, GLFW_CURSOR,
                       show_demo_window ? GLFW_CURSOR_NORMAL
                                        : GLFW_CURSOR_DISABLED);
      menuToggleReleased = false;
    }

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
      menuToggleReleased = true;
    }

    double now = glfwGetTime();
    float dt = (float)(now - lastTime);
    lastTime = now;

    if (!show_demo_window)
      player.update(window, mouse, dt);

    float yaw = player.angle * DEG2RAD;
    float pitch = player.pitch * DEG2RAD;

    renderer.light.direction =
        normalize({std::sin(yaw) * std::cos(pitch),
                   std::cos(yaw) * std::cos(pitch), std::sin(pitch)});

    renderer.clear(RAYWHITE);
    renderer.drawMeshes(scene, player);
    renderer.present(window);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw UI
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

    frames++;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
