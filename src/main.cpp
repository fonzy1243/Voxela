#include "glm/fwd.hpp"
#define GLFW_INCLUDE_NONE
#define GLM_ENABLE_EXPERIMENTAL

#include <GLFW/glfw3.h>
#include <camera.h>
#include <chunk.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <shaders.h>

Shader *cube_shader;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void process_input(GLFWwindow *window);

int screen_height = 900;
int screen_width = 1600;

Camera camera(glm::vec3(0.0, 0.0, -2.0));
float last_X = screen_width / 2.0f;
float last_Y = screen_height / 2.0f;
bool first_mouse = true;

float delta_time = 0.0f;
float last_frame = 0.0f;

int main() {
  float cube_vertices[] = {
      -1.0, 1.0,  1.0,  // F top left
      1.0,  1.0,  1.0,  // F top right
      1.0,  -1.0, 1.0,  // F bottom right
      -1.0, -1.0, 1.0,  // F bottom left
      -1.0, 1.0,  -1.0, // B top left
      1.0,  1.0,  -1.0, // B top right
      1.0,  -1.0, -1.0, // B bottom right
      -1.0, -1.0, -1.0  // B bottom left
  };

  int cube_indices[] = {
      0, 3, 2, // Front
      2, 1, 0, // Front
      1, 5, 6, // Right
      6, 2, 1, // Right
      5, 4, 7, // Left
      7, 6, 5, // Left
      4, 7, 3, // Back
      3, 0, 4, // Back
      4, 5, 1, // Top
      1, 0, 4, // Top
      3, 2, 6, // Bottom
      6, 7, 3, // Bottom
  };

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(screen_width, screen_height, "Voxela", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD:" << std::endl;
    return -1;
  }

  cube_shader = new Shader("../src/shaders/screen_shader.vs.glsl",
                           "../src/shaders/screen_shader.fs.glsl");

  Chunk orig_chunk(glm::vec3(0));
  Chunk new_chunk(glm::vec3(1, -1, 0));
  Chunk far_chunk(glm::vec3(2, 1, 0));

  glEnable(GL_DEPTH_TEST);

  // TODO: Change back to fill
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    // frame time logic
    float current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    // input
    process_input(window);

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    orig_chunk.render(camera);
    new_chunk.render(camera);
    far_chunk.render(camera);

    // swap buffers and call events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  delete cube_shader;

  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos_in, double ypos_in) {
  float xpos = static_cast<float>(xpos_in);
  float ypos = static_cast<float>(ypos_in);

  if (first_mouse) {
    last_X = xpos;
    last_Y = ypos;
    first_mouse = false;
  }

  float xoffset = xpos - last_X;
  float yoffset = last_Y - ypos;

  last_X = xpos;
  last_Y = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  // camera movement
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.ProcessKeyboard(FORWARD, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.ProcessKeyboard(LEFT, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.ProcessKeyboard(BACKWARD, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.ProcessKeyboard(RIGHT, delta_time);
  }
}
