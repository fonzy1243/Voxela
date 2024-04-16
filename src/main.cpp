#define GLFW_INCLUDE_NONE
#define GLM_ENABLE_EXPERIMENTAL

#include <GLFW/glfw3.h>
#include <camera.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <shaders.h>

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
      -0.5, 0.5,  0.5,  // F top left
      0.5,  0.5,  0.5,  // F top right
      0.5,  -0.5, 0.5,  // F bottom right
      -0.5, -0.5, 0.5,  // F bottom left
      -0.5, 0.5,  -0.5, // B top left
      0.5,  0.5,  -0.5, // B top right
      0.5,  -0.5, -0.5, // B bottom right
      -0.5, -0.5, -0.5  // B bottom left
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

  Shader cube_shader("../src/shaders/screen_shader.vs.glsl",
                     "../src/shaders/screen_shader.fs.glsl");

  unsigned int cube_vao, cube_vbo, cube_ebo;

  glGenVertexArrays(1, &cube_vao);

  glGenBuffers(1, &cube_vbo);
  glGenBuffers(1, &cube_ebo);

  glBindVertexArray(cube_vao);

  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glEnable(GL_DEPTH_TEST);

  // TODO: Change back to fill
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */

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

    cube_shader.use();

    // pass projection matrix
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), (float)screen_width / (float)screen_height,
        0.1f, 100.0f);
    cube_shader.set_mat4("projection", projection);

    // pass view transform matrix
    glm::mat4 view = camera.GetViewMatrix();
    cube_shader.set_mat4("view", view);

    // identity matrix
    glm::mat4 model = glm::mat4(1.0f);
    cube_shader.set_mat4("model", model);

    glBindVertexArray(cube_vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // swap buffers and call events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

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
