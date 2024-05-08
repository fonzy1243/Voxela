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
  Chunk(glm::ivec3 coordinates);
  Chunk(glm::ivec3 coordinates, std::vector<float>);
  ~Chunk();
  void update(float dt);
  const void render();
  void set_cube(int x, int y, int z, bool active);
  bool is_loaded();
  bool is_setup();
  void load();
  void unload();
  void setup();

private:
  unsigned int m_rendered_blocks = 0;
  unsigned int faces = 0;
  bool m_is_loaded = false;
  bool m_is_setup = false;
  Mesh *m_voxel_mesh = nullptr;
  glm::ivec3 m_position;
  BlockArray m_blocks;

  void create_cube(std::vector<Vertex> &vertices,
                   std::vector<unsigned int> &indices,
                   std::array<bool, 6> occluded, int x, int y, int z);
};

#endif
