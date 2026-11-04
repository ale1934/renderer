#include "math3d.hpp"
#include "mesh.hpp"
#include "player.hpp"
#include "primitives.hpp"
#include "renderer.hpp"

#include <GLFW/glfw3.h>
#include <cstdio>
#include <string>
#include <vector>

// Imgui
#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "../external/imgui/backends/imgui_impl_opengl3.h"
#include <imgui.h>

constexpr int SCREENWIDTH = 1440;
constexpr int SCREENHEIGHT = 960;

static void drawFpsOverlay(int fps) {
  const float padding = 10.0f;
  ImGuiIO &io = ImGui::GetIO();

  ImVec2 windowPos(io.DisplaySize.x - padding, padding);
  ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));
  ImGui::SetNextWindowBgAlpha(0.35f);

  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoInputs; // clicks pass through

  if (ImGui::Begin("FPS Overlay", nullptr, flags)) {
    ImGui::Text("FPS: %d", fps);
  }
  ImGui::End();
}

static void drawMenu(std::vector<Mesh> &scene) {
  static int selectedMesh = -1;
  static int renamingMesh = -1;
  static char renameBuf[128];

  ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);

  ImGui::Begin("Mesh Editor");

  ImVec2 treeSize(0, ImGui::GetContentRegionAvail().y * 0.6f);
  if (ImGui::BeginChild("TreeRegion", treeSize, ImGuiChildFlags_Borders)) {
    for (int i = 0; i < scene.size(); i++) {
      ImGui::PushID(i);

      if (renamingMesh == i) {
        ImGui::SetNextItemWidth(-FLT_MIN);
        bool commit = ImGui::InputText("##rename", renameBuf, sizeof(renameBuf),
                                       ImGuiInputTextFlags_EnterReturnsTrue |
                                           ImGuiInputTextFlags_AutoSelectAll);
        bool cancel = ImGui::IsKeyPressed(ImGuiKey_Escape);
        bool lostFocus = ImGui::IsItemDeactivated();

        if (cancel) {
          renamingMesh = -1;
        } else if (commit || lostFocus) {
          scene[i].name = renameBuf;
          renamingMesh = -1;
        }
      } else {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf |
                                   ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                   ImGuiTreeNodeFlags_SpanAvailWidth;
        if (selectedMesh == i)
          flags |= ImGuiTreeNodeFlags_Selected;

        ImGui::TreeNodeEx((void *)(intptr_t)i, flags, "%s",
                          scene[i].name.c_str());

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
          selectedMesh = i;
        }
        if (ImGui::IsItemHovered() &&
            ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
          renamingMesh = i;
          std::snprintf(renameBuf, sizeof(renameBuf), "%s",
                        scene[i].name.c_str());
          ImGui::SetKeyboardFocusHere(); // focus the box on next frame
        }
        if (ImGui::BeginPopupContextItem("RowContext")) {
          if (ImGui::MenuItem("Rename")) {
            renamingMesh = i;
            std::snprintf(renameBuf, sizeof(renameBuf), "%s",
                          scene[i].name.c_str());
            ImGui::SetKeyboardFocusHere();
          }
          ImGui::EndPopup();
        }
      }
      ImGui::PopID();
    }
  }

  if (ImGui::BeginPopupContextWindow("TreeContext",
                                     ImGuiPopupFlags_MouseButtonRight |
                                         ImGuiPopupFlags_NoOpenOverItems)) {
    if (ImGui::BeginMenu("Add Mesh")) {
      if (ImGui::MenuItem("Rect")) {
        scene.push_back(makeRect({0, 0, 0}, {40, 40, 40}));
        scene.back().name = "Rect " + std::to_string(scene.size() - 1);
        selectedMesh = (int)scene.size() - 1;
      }
      if (ImGui::MenuItem("Pyramid")) {
        scene.push_back(makePyramid({0, 0, 0}, {40, 40}));
        scene.back().name = "Pyramid " + std::to_string(scene.size() - 1);
        selectedMesh = (int)scene.size() - 1;
      }
      if (ImGui::MenuItem("Sphere")) {
        scene.push_back(makeSphere({0, 0, 0}, 64, 20));
        scene.back().name = "Sphere " + std::to_string(scene.size() - 1);
        selectedMesh = (int)scene.size() - 1;
      }
      ImGui::EndMenu();
    }
    ImGui::EndPopup();
  }

  ImGui::EndChild();

  ImGui::Separator();

  // Mesh inspector
  if (selectedMesh >= 0 && selectedMesh < scene.size()) {

    Mesh &mesh = scene[selectedMesh];

    ImGui::Text("Editing: %s", mesh.name.c_str());

    ImGui::Separator();

    // Position
    ImGui::Text("Position");

    ImGui::DragFloat3("Position", &mesh.position.x, 0.1f);

    // Color
    ImGui::Text("Color");

    float color[4] = {mesh.color.r / 255.0f, mesh.color.g / 255.0f,
                      mesh.color.b / 255.0f, mesh.color.a / 255.0f};

    if (ImGui::ColorEdit4("Mesh Color", color)) {
      mesh.color.r = (unsigned char)(color[0] * 255.0f);
      mesh.color.g = (unsigned char)(color[1] * 255.0f);
      mesh.color.b = (unsigned char)(color[2] * 255.0f);
      mesh.color.a = (unsigned char)(color[3] * 255.0f);
    }

  } else {
    ImGui::TextDisabled("Select a mesh to edit.");
  }

  ImGui::End();
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

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  bool show_menu = false;
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
      makeRect({0, 0, 0}, {40, 40, 40}),
      makePyramid({0, 0, 0}, {40, 40}),
      makeRect({0, 0, 0}, {40, 40, 40}),
      makeSphere({0, 0, 0}, 64, 20),
  };

  // Testing out color applications
  scene[0].color = RED;
  scene[1].color = BLUE;
  scene[2].color = GREEN;
  scene[3].color = ORANGE;

  // Setting Positions
  scene[0].position = {0, 200, -20};
  scene[1].position = {60, 200, -20};
  scene[2].position = {120, 200, -20};
  scene[3].position = {200, 220, 0};

  double lastTime = glfwGetTime();
  double menuTimer = 0.0;
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
      show_menu = !show_menu;

      glfwSetInputMode(window, GLFW_CURSOR,
                       show_menu ? GLFW_CURSOR_CAPTURED : GLFW_CURSOR_DISABLED);

      mouse.dx = 0;
      mouse.dy = 0;

      menuToggleReleased = false;
    }

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
      menuToggleReleased = true;
    }

    double now = glfwGetTime();
    float dt = (float)(now - lastTime);
    lastTime = now;

    if (!show_menu)
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
    if (show_menu)
      drawMenu(scene);

    drawFpsOverlay(fps);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    frames++;
    menuTimer += dt;
    if (menuTimer >= 0.5) {
      fps = (int)(frames / menuTimer);
      frames = 0;
      menuTimer = 0.0f;
    }
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
