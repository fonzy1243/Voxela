#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <shaders.h>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color;
};

class Mesh {
public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
  void draw(Shader &shader);

private:
  unsigned int VAO, VBO, EBO;

  void setup_mesh();
};

#endif
