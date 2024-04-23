#include <camera.h>
#include <chunk.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

extern Shader *cube_shader;

extern int screen_height;
extern int screen_width;

int max_offset = 0;

extern const int chunk_size;
extern const int half_chunk_size;
extern const int buff_size;
constexpr float block_size = 0.5;

extern BlockArray blocks;

Chunk::Chunk(glm::ivec3 coordinates) { m_position = coordinates; }

Chunk::Chunk(glm::ivec3 coordinates, BlockArray &blocks) {
  m_blocks = blocks;
  m_position = coordinates;

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
        create_cube(vertices, indices, start.x + x, start.y + y, start.z + z);
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
        create_cube(vertices, indices, x, y, z);
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

  for (int x = 0; x < chunk_size; x++) {
    for (int y = 0; y < chunk_size; y++) {
      for (int z = 0; z < chunk_size; z++) {
        if (m_blocks[x][y][z].is_active()) {
          create_cube(vertices, indices, start.x + x, start.y + y, start.z + z);
        }
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

  unsigned int offset = 24 * m_rendered_blocks;

  // Front face
  normal = glm::vec3(0.0f, 0.0f, 1.0f);
  Vertex top_left, bottom_right, bottom_left, top_right;
  // left triangle
  glm::vec4 front_color(0, 0, 1.0f, 1.0f);
  top_left = create_vertex(front_tleft, normal, front_color);      // 2
  bottom_right = create_vertex(front_bright, normal, front_color); // 1
  bottom_left = create_vertex(front_bleft, normal, front_color);   // 0

  // right triangle
  top_right = create_vertex(front_tright, normal, front_color); // 3

  indices.insert(indices.end(), {offset + 0, offset + 1, offset + 3, offset + 0,
                                 offset + 3, offset + 2});
  vertices.insert(vertices.end(),
                  {bottom_left, bottom_right, top_left, top_right});

  // Right face
  normal = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec4 right_color(0, 1.0f, 0, 1.0f);
  // left triangle
  top_left = create_vertex(front_tright, normal, right_color);    // 6
  bottom_right = create_vertex(back_bright, normal, right_color); // 5
  bottom_left = create_vertex(front_bright, normal, right_color); // 4

  // right triangle
  top_right = create_vertex(back_tright, normal, right_color); // 7

  indices.insert(indices.end(), {offset + 4, offset + 5, offset + 7, offset + 4,
                                 offset + 5, offset + 6});
  vertices.insert(vertices.end(),
                  {bottom_left, bottom_right, top_left, top_right});

  // Back face
  normal = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec4 back_color(1.0f, 0, 0, 1.0f);
  // left triangle
  top_left = create_vertex(back_tright, normal, back_color);    // 10
  bottom_right = create_vertex(back_bleft, normal, back_color); // 9
  bottom_left = create_vertex(back_bright, normal, back_color); // 8

  // right triangle
  top_right = create_vertex(back_tleft, normal, back_color); // 11

  indices.insert(indices.end(), {offset + 8, offset + 9, offset + 11,
                                 offset + 8, offset + 11, offset + 10});
  vertices.insert(vertices.end(),
                  {bottom_left, bottom_right, top_left, top_right});

  // Left face
  normal = glm::vec3(-1.0f, 0.0f, 0.0f);
  glm::vec4 left_color(0, 0.5f, 0.5f, 1.0f);
  // left triangle
  top_left = create_vertex(back_tleft, normal, left_color);      // 14
  bottom_right = create_vertex(front_bleft, normal, left_color); // 13
  bottom_left = create_vertex(back_bleft, normal, left_color);   // 12

  // right triangle
  top_right = create_vertex(front_tleft, normal, left_color); // 15

  indices.insert(indices.end(), {offset + 12, offset + 13, offset + 15,
                                 offset + 12, offset + 15, offset + 14});
  vertices.insert(vertices.end(),
                  {bottom_left, bottom_right, top_left, top_right});

  // Top face
  normal = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec4 top_color(0.5f, 0.5f, 0.0f, 1.0f);
  // left triangle
  top_left = create_vertex(back_tleft, normal, top_color);       // 18
  bottom_right = create_vertex(front_tright, normal, top_color); // 17
  bottom_left = create_vertex(front_tleft, normal, top_color);   // 16

  // right triangle
  top_right = create_vertex(back_tright, normal, top_color); // 19
  indices.insert(indices.end(), {offset + 16, offset + 17, offset + 19,
                                 offset + 16, offset + 19, offset + 18});
  vertices.insert(vertices.end(),
                  {bottom_left, bottom_right, top_left, top_right});

  // Bottom face
  normal = glm::vec3(0.0f, -1.0f, 0.0f);
  glm::vec4 bottom_color(0.5f, 0.0f, 0.5f, 1.0f);
  // left triangle
  top_left = create_vertex(front_bleft, normal, bottom_color);     // 22
  bottom_right = create_vertex(back_bright, normal, bottom_color); // 21
  bottom_left = create_vertex(back_bleft, normal, bottom_color);   // 20

  // right triangle
  top_right = create_vertex(front_bright, normal, bottom_color); // 23
  indices.insert(indices.end(), {offset + 20, offset + 21, offset + 23,
                                 offset + 20, offset + 23, offset + 22});
  vertices.insert(vertices.end(),
                  {bottom_left, bottom_right, top_left, top_right});

  m_rendered_blocks++;
}

void Chunk::set_cube(int x, int y, int z, bool active) {
  m_blocks[x][y][z].set_active(active);
}
