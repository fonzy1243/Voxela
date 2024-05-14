#include <array>
#include <camera.h>
#include <chunk.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

extern Shader *cube_shader;

extern int screen_height;
extern int screen_width;

extern const int chunk_size;
extern const int half_chunk_size;
extern const int buff_size;
constexpr float block_size = 0.5;

extern BlockArray blocks;

Chunk::Chunk(glm::ivec3 coordinates) { m_position = coordinates; }

Chunk::Chunk(glm::ivec3 coordinates, std::vector<float> noise) {
  m_position = coordinates;

  for (int x = 0; x < chunk_size; x++) {
    for (int z = 0; z < chunk_size; z++) {
      // Calculate the global x and z positions.
      int globalX = m_position.x * chunk_size + x;
      int globalZ = m_position.z * chunk_size + z;
      // Calculate the index into the noise array.
      int noiseIndex = globalX * 8000 + globalZ;
      // Scale the noise value to the range of the chunk height.
      float height = noise[noiseIndex] * chunk_size;
      // Fill the blocks in the chunk up to the calculated height.
      for (int y = 0; y < height; y++) {
        m_blocks[x][y][z].set_active(true);
      }
    }
  }

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  glm::ivec3 start(coordinates * chunk_size);

  bool temp = false;
  for (int x = 0; x < chunk_size; x++) {
    for (int y = 0; y < chunk_size; y++) {
      for (int z = 0; z < chunk_size; z++) {
        if (!m_blocks[x][y][z].is_active()) {
          continue;
        }

        bool x_negative = temp;
        if (x > 0) {
          x_negative = m_blocks[x - 1][y][z].is_active();
        }
        bool x_postive = temp;
        if (x < chunk_size - 1) {
          x_postive = m_blocks[x + 1][y][z].is_active();
        }

        bool y_negative = temp;
        if (y > 0) {
          y_negative = m_blocks[x][y - 1][z].is_active();
        }
        bool y_postive = temp;
        if (y < chunk_size - 1) {
          y_postive = m_blocks[x][y + 1][z].is_active();
        }

        bool z_negative = temp;
        if (z > 0) {
          z_negative = m_blocks[x][y][z - 1].is_active();
        }

        bool z_postive = temp;
        if (z < chunk_size - 1) {
          z_postive = m_blocks[x][y][z + 1].is_active();
        }

        if (x_negative && x_postive && y_negative && y_postive && z_negative &&
            z_postive) {
          continue;
        }

        std::array<bool, 6> occluded = {z_postive,  x_postive, z_negative,
                                        x_negative, y_postive, y_negative};
        create_cube(vertices, indices, occluded, start.x + x, start.y + y,
                    start.z + z);
      }
    }
  }

  m_voxel_mesh = new Mesh(vertices, indices);
}

Chunk::~Chunk() {
  delete m_voxel_mesh;
  std::cout << "delete" << std::endl;
}

void Chunk::update(float dt) {
  m_rendered_blocks = 0;
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  glm::ivec3 start(m_position * chunk_size);

  bool temp = true;
  for (int x = 0; x < chunk_size; x++) {
    for (int y = 0; y < chunk_size; y++) {
      for (int z = 0; z < chunk_size; z++) {
        if (!m_blocks[x][y][z].is_active()) {
          continue;
        }

        bool x_negative = temp;
        if (x > 0) {
          x_negative = m_blocks[x - 1][y][z].is_active();
        }
        bool x_postive = temp;
        if (x < chunk_size - 1) {
          x_postive = m_blocks[x + 1][y][z].is_active();
        }

        bool y_negative = temp;
        if (y > 0) {
          y_negative = m_blocks[x][y - 1][z].is_active();
        }
        bool y_postive = temp;
        if (y < chunk_size - 1) {
          y_postive = m_blocks[x][y + 1][z].is_active();
        }

        bool z_negative = temp;
        if (z > 0) {
          x_negative = m_blocks[x][y][z - 1].is_active();
        }

        bool z_postive = temp;
        if (z < chunk_size - 1) {
          z_postive = m_blocks[x][y][z + 1].is_active();
        }

        if (x_negative && x_postive && y_negative && y_postive && z_negative &&
            z_postive) {
          continue;
        }

        std::array<bool, 6> occluded = {z_postive,  x_postive, z_negative,
                                        x_negative, y_postive, y_negative};
        create_cube(vertices, indices, occluded, x, y, z);
      }
    }
  }

  m_voxel_mesh = new Mesh(vertices, indices);
}

const void Chunk::render() { m_voxel_mesh->draw(*cube_shader); }

bool Chunk::is_loaded() { return m_is_loaded; }
bool Chunk::is_setup() { return m_is_setup; }
void Chunk::load() { m_is_loaded = true; }
void Chunk::unload() { m_is_loaded = false; }

void Chunk::setup() {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  glm::ivec3 start(m_position * chunk_size);

  bool temp = false;
  for (int x = 0; x < chunk_size; x++) {
    for (int y = 0; y < chunk_size; y++) {
      for (int z = 0; z < chunk_size; z++) {
        if (!m_blocks[x][y][z].is_active()) {
          continue;
        }
        bool x_negative = temp;
        if (x > 0) {
          x_negative = m_blocks[x - 1][y][z].is_active();
        }
        bool x_postive = temp;
        if (x < chunk_size - 1) {
          x_postive = m_blocks[x + 1][y][z].is_active();
        }

        bool y_negative = temp;
        if (y > 0) {
          y_negative = m_blocks[x][y - 1][z].is_active();
        }
        bool y_postive = temp;
        if (y < chunk_size - 1) {
          y_postive = m_blocks[x][y + 1][z].is_active();
        }

        bool z_negative = temp;
        if (z > 0) {
          z_negative = m_blocks[x][y][z - 1].is_active();
        }

        bool z_postive = temp;
        if (z < chunk_size - 1) {
          z_postive = m_blocks[x][y][z + 1].is_active();
        }

        if (x_negative && x_postive && y_negative && y_postive && z_negative &&
            z_postive) {
          continue;
        }

        std::array<bool, 6> occluded = {z_postive,  x_postive, z_negative,
                                        x_negative, y_postive, y_negative};
        create_cube(vertices, indices, occluded, start.x + x, start.y + y,
                    start.z + z);
      }
    }
  }

  m_voxel_mesh = new Mesh(vertices, indices);
  m_is_setup = true;
}

Vertex create_vertex(glm::vec3 position, glm::vec3 normal, glm::vec4 color) {
  Vertex v;
  v.position = position;
  v.normal = normal;
  v.color = color;
  return v;
}

void Chunk::create_cube(std::vector<Vertex> &vertices,
                        std::vector<unsigned int> &indices,
                        std::array<bool, 6> occluded, int x, int y, int z) {

  std::vector<glm::vec3> cube_vertices = {
      // Front
      {x - block_size, y - block_size, z + block_size},
      {x + block_size, y - block_size, z + block_size},
      {x + block_size, y + block_size, z + block_size},
      {x - block_size, y + block_size, z + block_size},
      // Back
      {x - block_size, y - block_size, z - block_size},
      {x + block_size, y - block_size, z - block_size},
      {x + block_size, y + block_size, z - block_size},
      {x - block_size, y + block_size, z - block_size},
  };

  std::vector<glm::vec3> normals = {
      {0, 0, 1},  // Front
      {1, 0, 0},  // Right
      {0, 0, -1}, // Back
      {-1, 0, 0}, // Left
      {0, 1, 0},  // Top
      {0, -1, 0}, // Bottom
  };

  std::vector<glm::vec4> colors = {
      {0, 0, 1, 1},     // Front
      {0, 1, 0, 1},     // Right
      {1, 0, 0, 1},     // Back
      {0, 0.5, 0.5, 1}, // Left
      {0.5, 0.5, 0, 1}, // Top
      {0.5, 0, 0.5, 1}, // Bottom
  };

  for (int i = 0; i < 6; i++) {
    if (occluded[i]) {
      continue;
    }

    Vertex top_left, bottom_right, bottom_left, top_right;

    glm::vec3 normal = normals[i];
    glm::vec4 color = colors[i];

    switch (i) {
    // Front
    case 0:
      bottom_left = create_vertex(cube_vertices[0], normal, color);  // 0
      bottom_right = create_vertex(cube_vertices[1], normal, color); // 1
      top_left = create_vertex(cube_vertices[3], normal, color);     // 2
      top_right = create_vertex(cube_vertices[2], normal, color);    // 3
      break;
    // Right
    case 1:
      bottom_left = create_vertex(cube_vertices[1], normal, color);  // 0
      bottom_right = create_vertex(cube_vertices[5], normal, color); // 1
      top_left = create_vertex(cube_vertices[2], normal, color);     // 2
      top_right = create_vertex(cube_vertices[6], normal, color);    // 3
      break;
    // Back
    case 2:
      bottom_left = create_vertex(cube_vertices[5], normal, color);  // 0
      bottom_right = create_vertex(cube_vertices[4], normal, color); // 1
      top_left = create_vertex(cube_vertices[6], normal, color);     // 2
      top_right = create_vertex(cube_vertices[7], normal, color);    // 3
      break;
    // Left
    case 3:
      bottom_left = create_vertex(cube_vertices[4], normal, color);  // 0
      bottom_right = create_vertex(cube_vertices[0], normal, color); // 1
      top_left = create_vertex(cube_vertices[7], normal, color);     // 2
      top_right = create_vertex(cube_vertices[3], normal, color);    // 3
      break;
    // Top
    case 4:
      bottom_left = create_vertex(cube_vertices[3], normal, color);  // 0
      bottom_right = create_vertex(cube_vertices[2], normal, color); // 1
      top_left = create_vertex(cube_vertices[7], normal, color);     // 2
      top_right = create_vertex(cube_vertices[6], normal, color);    // 3
      break;
    // Bottom
    case 5:
      bottom_left = create_vertex(cube_vertices[4], normal, color);  // 0
      bottom_right = create_vertex(cube_vertices[5], normal, color); // 1
      top_left = create_vertex(cube_vertices[0], normal, color);     // 2
      top_right = create_vertex(cube_vertices[1], normal, color);    // 3
      break;
    }

    indices.insert(indices.end(), {0 + faces, 1 + faces, 3 + faces, 0 + faces,
                                   3 + faces, 2 + faces});
    vertices.insert(vertices.end(),
                    {bottom_left, bottom_right, top_left, top_right});

    faces += 4;
  }

  m_rendered_blocks++;
}

void Chunk::set_cube(int x, int y, int z, bool active) {
  m_blocks[x][y][z].set_active(active);
}
