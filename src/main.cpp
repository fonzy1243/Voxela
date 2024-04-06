#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <shaders.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

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

  int screen_height = 900;
  int screen_width = 1600;

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

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD:" << std::endl;
    return -1;
  }

  glViewport(0, 0, screen_width, screen_height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

  while (!glfwWindowShouldClose(window)) {
    process_input(window);

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    screen_shader.use();
    glfwGetWindowSize(window, &screen_width, &screen_height);
    screen_shader.set_int("screen_width", screen_width);
    screen_shader.set_int("screen_height", screen_height);
    glBindVertexArray(screen_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
