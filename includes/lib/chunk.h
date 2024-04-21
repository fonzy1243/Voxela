#ifndef CHUNK_H
#define CHUNK_H

#include <camera.h>
#include <glm/glm.hpp>
#include <shaders.h>

static const float cube_vertices[] = {
    -1.0, 1.0,  1.0,  // F top left
    1.0,  1.0,  1.0,  // F top right
    1.0,  -1.0, 1.0,  // F bottom right
    -1.0, -1.0, 1.0,  // F bottom left
    -1.0, 1.0,  -1.0, // B top left
    1.0,  1.0,  -1.0, // B top right
    1.0,  -1.0, -1.0, // B bottom right
    -1.0, -1.0, -1.0  // B bottom left
};

static const int cube_indices[] = {
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

class Chunk {
public:
  Chunk(glm::vec3 coordinates);
  const void render(Camera camera);

private:
  unsigned int vao, vbo, ebo, instance_vbo;
  glm::vec3 translations[32768];
};

#endif
