#include <camera.h>
#include <chunk.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <utility>
#include <vector>

extern Shader *cube_shader;

extern int screen_height;
extern int screen_width;

extern const int chunk_size;
extern const int half_chunk_size;
extern const int buff_size;
constexpr float block_size = 0.5;

Chunk::Chunk(glm::ivec3 coordinates, BlockArray &blocks) {
  m_blocks = std::move(blocks);

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  glGenBuffers(1, &instance_vbo);

  glm::ivec3 start(coordinates * chunk_size);

  for (int x = 0; x < chunk_size; x++) {
    for (int y = 0; y < chunk_size; y++) {
      for (int z = 0; z < chunk_size; z++) {
        if (m_blocks[x][y][z].is_active()) {
          create_cube(vertices, indices, start.x + x, start.y + y, start.z + z);
        }
      }
    }
  }

  voxel_mesh = new Mesh(vertices, indices);
}

const void Chunk::render() { voxel_mesh->draw(*cube_shader); }

Vertex create_vertex(glm::vec3 position, glm::vec3 normal, glm::vec4 color) {
  Vertex v;
  v.position = position;
  v.normal = normal;
  v.color = color;
  return v;
}

void Chunk::create_cube(std::vector<Vertex> &vertices,
                        std::vector<unsigned int> &indices, int x, int y,
                        int z) {
  glm::vec4 color(1.0f);

  // Front vertices
  glm::vec3 front_bleft(x - block_size, y - block_size, z + block_size);
  glm::vec3 front_bright(x + block_size, y - block_size, z + block_size);
  glm::vec3 front_tright(x + block_size, y + block_size, z + block_size);
  glm::vec3 front_tleft(x - block_size, y + block_size, z + block_size);

  // Back vertices
  glm::vec3 back_bleft(x - block_size, y - block_size, z - block_size);
  glm::vec3 back_bright(x + block_size, y - block_size, z - block_size);
  glm::vec3 back_tright(x + block_size, y + block_size, z - block_size);
  glm::vec3 back_tleft(x - block_size, y + block_size, z - block_size);

  glm::vec3 normal;

  // Front face
  normal = glm::vec3(0.0f, 0.0f, 1.0f);
  Vertex top_left, bottom_right, bottom_left, top_right;
  // left triangle
  glm::vec4 front_color(0, 0, 1.0f, 1.0f);
  top_left = create_vertex(front_tleft, normal, front_color);
  bottom_right = create_vertex(front_bright, normal, front_color);
  bottom_left = create_vertex(front_bleft, normal, front_color);

  // right triangle
  top_right = create_vertex(front_tright, normal, front_color);

  // offset the indices by the current amount of vertices
  indices.insert(
      indices.end(),
      {0 + (unsigned int)vertices.size(), 1 + (unsigned int)vertices.size(),
       2 + (unsigned int)vertices.size(), 0 + (unsigned int)vertices.size(),
       3 + (unsigned int)vertices.size(), 2 + (unsigned int)vertices.size()});
  vertices.insert(vertices.end(),
                  {top_left, bottom_left, bottom_right, top_right});

  // Back face
  normal = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec4 back_color(1.0f, 0, 0, 1.0f);
  // left triangle
  top_left = create_vertex(back_tleft, normal, back_color);
  bottom_right = create_vertex(back_bright, normal, back_color);
  bottom_left = create_vertex(back_bleft, normal, back_color);

  // right triangle
  top_right = create_vertex(back_tright, normal, back_color);

  indices.insert(
      indices.end(),
      {0 + (unsigned int)vertices.size(), 1 + (unsigned int)vertices.size(),
       2 + (unsigned int)vertices.size(), 0 + (unsigned int)vertices.size(),
       3 + (unsigned int)vertices.size(), 1 + (unsigned int)vertices.size()});
  vertices.insert(vertices.end(),
                  {top_left, bottom_right, bottom_left, top_right});
  // Right face
  normal = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec4 right_color(0, 1.0f, 0, 1.0f);
  // left triangle
  top_left = create_vertex(front_tright, normal, right_color);
  bottom_right = create_vertex(back_bright, normal, right_color);
  bottom_left = create_vertex(front_bright, normal, right_color);

  // right triangle
  top_right = create_vertex(back_tright, normal, right_color);
  indices.insert(
      indices.end(),
      {0 + (unsigned int)vertices.size(), 1 + (unsigned int)vertices.size(),
       2 + (unsigned int)vertices.size(), 0 + (unsigned int)vertices.size(),
       3 + (unsigned int)vertices.size(), 2 + (unsigned int)vertices.size()});
  vertices.insert(vertices.end(),
                  {top_left, bottom_left, bottom_right, top_right});

  // Left face
  normal = glm::vec3(-1.0f, 0.0f, 0.0f);
  glm::vec4 left_color(0, 0.5f, 0.5f, 1.0f);
  // left triangle
  top_left = create_vertex(back_tleft, normal, left_color);
  bottom_right = create_vertex(front_bleft, normal, left_color);
  bottom_left = create_vertex(back_bleft, normal, left_color);

  // right triangle
  top_right = create_vertex(front_tleft, normal, left_color);
  indices.insert(
      indices.end(),
      {0 + (unsigned int)vertices.size(), 1 + (unsigned int)vertices.size(),
       2 + (unsigned int)vertices.size(), 0 + (unsigned int)vertices.size(),
       3 + (unsigned int)vertices.size(), 2 + (unsigned int)vertices.size()});
  vertices.insert(vertices.end(),
                  {top_left, bottom_left, bottom_right, top_right});

  // Top face
  normal = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec4 top_color(0.5f, 0.5f, 0.0f, 1.0f);
  // left triangle
  top_left = create_vertex(back_tleft, normal, top_color);
  bottom_right = create_vertex(front_tright, normal, top_color);
  bottom_left = create_vertex(front_tleft, normal, top_color);

  // right triangle
  top_right = create_vertex(back_tright, normal, top_color);
  indices.insert(
      indices.end(),
      {0 + (unsigned int)vertices.size(), 1 + (unsigned int)vertices.size(),
       2 + (unsigned int)vertices.size(), 0 + (unsigned int)vertices.size(),
       3 + (unsigned int)vertices.size(), 2 + (unsigned int)vertices.size()});
  vertices.insert(vertices.end(),
                  {top_left, bottom_left, bottom_right, top_right});

  // Bottom face
  normal = glm::vec3(0.0f, -1.0f, 0.0f);
  glm::vec4 bottom_color(0.5f, 0.0f, 0.5f, 1.0f);
  // left triangle
  top_left = create_vertex(front_bleft, normal, bottom_color);
  bottom_right = create_vertex(back_bright, normal, bottom_color);
  bottom_left = create_vertex(back_bleft, normal, bottom_color);

  // right triangle
  top_right = create_vertex(back_bright, normal, bottom_color);
  indices.insert(
      indices.end(),
      {0 + (unsigned int)vertices.size(), 1 + (unsigned int)vertices.size(),
       2 + (unsigned int)vertices.size(), 0 + (unsigned int)vertices.size(),
       3 + (unsigned int)vertices.size(), 2 + (unsigned int)vertices.size()});
  vertices.insert(vertices.end(),
                  {top_left, bottom_left, bottom_right, top_right});
}

void Chunk::set_cube(int x, int y, int z, bool active) {
  m_blocks[x][y][z].set_active(active);
}
