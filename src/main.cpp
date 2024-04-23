#define GLFW_INCLUDE_NONE
#define GLM_ENABLE_EXPERIMENTAL

#include <GLFW/glfw3.h>
#include <camera.h>
#include <chunk.h>
#include <compute_shader.h>
#include <gl_errors.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <shaders.h>

Shader *cube_shader;
BlockArray blocks;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void process_input(GLFWwindow *window);

int screen_height = 900;
int screen_width = 1600;

Camera camera(glm::vec3(0.0, 0.0, 0.0));
float last_X = screen_width / 2.0f;
float last_Y = screen_height / 2.0f;
bool first_mouse = true;

float delta_time = 0.0f;
float last_frame = 0.0f;

int main() {
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

  for (int x = 0; x < 32; x++) {
    for (int y = 0; y < 32; y++) {
      for (int z = 0; z < 32; z++) {
        blocks[x][y][z].set_active(true);
      }
    }
  }

  std::vector<Chunk *> chunks;

  std::cout << "deleted?" << std::endl;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      Chunk *chunk = new Chunk(glm::ivec3(i, 0, j), blocks);
      chunks.push_back(chunk);
    }
  }
  std::cout << "deleted" << std::endl;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // TODO: Change back to fill
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    // frame time logic
    float current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    float fps = 1.0f / delta_time;
    std::cout << fps << std::endl;

    // input
    process_input(window);

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cube_shader->use();

    // pass projection matrix
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), (float)screen_width / (float)screen_height,
        0.1f, 500.0f);
    cube_shader->set_mat4("projection", projection);
    // pass view transform matrix
    glm::mat4 view = camera.GetViewMatrix();
    cube_shader->set_mat4("view", view);
    // identity matrix
    glm::mat4 model = glm::mat4(1.0f);
    cube_shader->set_mat4("model", model);

    for (int i = 0; i < chunks.size(); i++) {
      chunks[i]->render();
    }

    // swap buffers and call events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  delete cube_shader;
  for (Chunk *chunk : chunks) {
    delete chunk;
  }

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
