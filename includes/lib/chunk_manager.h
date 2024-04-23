#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <camera.h>
#include <chunk.h>
#include <glm/glm.hpp>
#include <vector>

typedef std::vector<Chunk *> ChunkList;

class ChunkManager {
public:
  ChunkManager(Camera camera);
  void update(float dt, glm::vec3 camera_pos, glm::vec3 camera_view);
  void update_load_list();
  void update_setup_list();
  void update_rebuild_list();
  void update_unload_list();
  void update_render_list();
  void update_flags_list();

private:
  ChunkList m_render_list;
  ChunkList m_load_list;
  ChunkList m_setup_list;
  ChunkList m_rebuild_list;
  ChunkList m_unload_list;
  ChunkList m_flags_list;

  glm::vec3 m_camera_pos;
  glm::vec3 m_camera_view;
};

#endif // !CHUNK_MANAGER_H
