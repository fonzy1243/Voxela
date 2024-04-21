#include <camera.h>
#include <chunk.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

extern Shader *cube_shader;

extern int screen_height;
extern int screen_width;

constexpr int chunk_size = 8;
constexpr int half_chunk_size = chunk_size / 2;
constexpr int buff_size = chunk_size * chunk_size * chunk_size;

Chunk::Chunk(glm::vec3 coordinates) {
  glGenBuffers(1, &instance_vbo);

  int i = 0;

  for (int z = 0; z < chunk_size - 1; z++) {
    for (int y = 0; y < chunk_size - 1; y++) {
      for (int x = 0; x < chunk_size - 1; x++) {
        glm::vec3 translation;
        translation.x =
            (float)x - half_chunk_size + (chunk_size * coordinates.x);
        translation.y =
            (float)y - half_chunk_size + (chunk_size * coordinates.y);
        translation.z =
            (float)z - half_chunk_size + (chunk_size * coordinates.z);
        translations[i++] = translation;
      }
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * buff_size, &translations[0],
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenVertexArrays(1, &vao);

  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices,
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glVertexAttribDivisor(1, 1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

const void Chunk::render(Camera camera) {
  cube_shader->use();

  // pass projection matrix
  glm::mat4 projection = glm::perspective(
      glm::radians(camera.Zoom), (float)screen_width / (float)screen_height,
      0.1f, 100.0f);
  cube_shader->set_mat4("projection", projection);
  // pass view transform matrix
  glm::mat4 view = camera.GetViewMatrix();
  cube_shader->set_mat4("view", view);
  // identity matrix
  glm::mat4 model = glm::mat4(1.0f);
  cube_shader->set_mat4("model", model);

  glBindVertexArray(vao);
  glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, buff_size);
}

/* void Chunk::set_shader(const Shader &shader) { cube_shader = shader; } */
