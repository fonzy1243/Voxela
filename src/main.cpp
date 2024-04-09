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

Camera camera(glm::vec3(0.0, 0.0, -2.5));
float last_X = screen_width / 2.0f;
float last_Y = screen_height / 2.0f;
bool first_mouse = true;

float delta_time = 0.0f;
float last_frame = 0.0f;

int main() {
  float screen_quad[] = {
      1.0f,  1.0f,  0.0f, // top right
      1.0f,  -1.0f, 0.0f, // bottom right
      -1.0f, -1.0f, 0.0f, // bottom left
      -1.0f, 1.0f,  0.0f, // top left
  };

  unsigned int screen_indices[] = {
      0, 1, 3, // top right triangle
      1, 2, 3  // bottom left triangle
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

  Shader screen_shader("../src/shaders/screen_shader.vs.glsl",
                       "../src/shaders/screen_shader.fs.glsl");

  unsigned int screen_vao, screen_vbo, screen_ebo;

  glGenVertexArrays(1, &screen_vao);

  glGenBuffers(1, &screen_vbo);
  glGenBuffers(1, &screen_ebo);

  glBindVertexArray(screen_vao);

  glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad), screen_quad,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screen_indices), screen_indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  std::cout << "FRONT: " << camera.GetDirection().x << ", "
            << camera.GetDirection().y << ", " << camera.GetDirection().z
            << std::endl;

  while (!glfwWindowShouldClose(window)) {
    // frame time logic
    float current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    // input
    process_input(window);

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    screen_shader.use();
    glfwGetWindowSize(window, &screen_width, &screen_height);
    screen_shader.set_int("screen_width", screen_width);
    screen_shader.set_int("screen_height", screen_height);

    screen_shader.use();
    screen_shader.set_vec3("camera_pos", camera.Position);

    screen_shader.use();
    screen_shader.set_vec3("camera_dir", camera.GetDirection());

    screen_shader.use();
    screen_shader.set_vec3("camera_up", camera.GetUp());

    glBindVertexArray(screen_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // swap buffers and call events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  std::cout << "FRONT: " << camera.GetDirection().x << ", "
            << camera.GetDirection().y << ", " << camera.GetDirection().z
            << std::endl;
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
