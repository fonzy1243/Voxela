#ifndef CHUNK_H
#define CHUNK_H

#include <array>
#include <block.h>
#include <camera.h>
#include <glm/glm.hpp>
#include <mesh.h>
#include <shaders.h>
#include <vector>

constexpr int chunk_size = 32;
constexpr int half_chunk_size = chunk_size / 2;
constexpr int buff_size = chunk_size * chunk_size * chunk_size;

typedef std::array<std::array<std::array<Block, chunk_size>, chunk_size>,
                   chunk_size>
    BlockArray;

class Chunk {
public:
  Chunk(glm::ivec3 coordinates, BlockArray &blocks);
  const void render();
  void set_cube(int x, int y, int z, bool active);

private:
  unsigned int vao, vbo, ebo, instance_vbo;
  Mesh *voxel_mesh = nullptr;
  unsigned int active_blocks = 0;
  glm::vec3 translations[32768];
  BlockArray m_blocks;

  void create_cube(std::vector<Vertex> &vertices,
                   std::vector<unsigned int> &indices, int x, int y, int z);
};

#endif
