#include "player.hpp"

void Player::update(GLFWwindow *window, MouseDelta &mouse, float dt) {
  constexpr float mouseSensitivity =
      0.1f; // Degrees per pixel of mouse movement.

  if (mouseWarmupFrames > 0) {
    // Ignore mouse movement while the window settles.
    mouseWarmupFrames--;
  } else {
    angle += (float)mouse.dx * mouseSensitivity;
    pitch -= (float)mouse.dy * mouseSensitivity;
  }
  mouse.dx = 0;
  mouse.dy = 0;

  pitch = std::clamp(pitch, -89.0f, 89.0f);

  float rad = angle * DEG2RAD;
  float dx = std::sin(rad);
  float dy = std::cos(rad);

  float speed = 100.0f * dt;

  if (isDown(window, GLFW_KEY_W)) {
    pos.x += dx * speed;
    pos.y += dy * speed;
  }
  if (isDown(window, GLFW_KEY_S)) {
    pos.x -= dx * speed;
    pos.y -= dy * speed;
  }
  if (isDown(window, GLFW_KEY_A)) {
    pos.x -= dy * speed;
    pos.y += dx * speed;
  }
  if (isDown(window, GLFW_KEY_D)) {
    pos.x += dy * speed;
    pos.y -= dx * speed;
  }
  if (isDown(window, GLFW_KEY_SPACE))
    pos.z += speed;
  if (isDown(window, GLFW_KEY_LEFT_SHIFT))
    pos.z -= speed;
}
