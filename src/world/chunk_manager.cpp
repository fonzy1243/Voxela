#include "glm/fwd.hpp"
#include <camera.h>
#include <chunk_manager.h>

// TODO: Work on chunk manager.

ChunkManager::ChunkManager(Camera camera) {
  ChunkList empty_list;

  m_render_list = empty_list;
  m_load_list = empty_list;
  m_setup_list = empty_list;
  m_rebuild_list = empty_list;
  m_unload_list = empty_list;
  m_flags_list = empty_list;

  m_camera_pos = camera.Position;
  m_camera_view = camera.Front;
}

void ChunkManager::update(float dt, glm::vec3 camera_pos,
                          glm::vec3 camera_view) {
  update_load_list();
  update_setup_list();
  update_rebuild_list();
  update_flags_list();
  update_unload_list();
  // TODO: Update visibility list
  if (m_camera_pos != camera_pos || m_camera_view != camera_view) {
    update_render_list();
  }
  m_camera_pos = camera_pos;
  m_camera_view = camera_view;
}

void ChunkManager::update_load_list() {
  int chunks_loaded = 0;

  ChunkList::iterator iterator;

  for (iterator = m_load_list.begin(); iterator != m_load_list.end();
       iterator++) {
    Chunk *chunk = (*iterator);
    if (!chunk->is_loaded()) {
      chunk->load();
      chunks_loaded++;
    }
  }

  m_load_list.clear();
}

void ChunkManager::update_setup_list() {
  ChunkList::iterator iterator;

  for (iterator = m_setup_list.begin(); iterator != m_setup_list.end();
       iterator++) {
    Chunk *chunk = (*iterator);
    if (chunk->is_loaded() && !chunk->is_setup()) {
      chunk->setup();
    }
  }

  m_setup_list.clear();
}

// TODO: Finish this
void ChunkManager::update_rebuild_list() {
  ChunkList::iterator iterator;
  int rebuilt_chunks;
  for (iterator = m_rebuild_list.begin(); iterator != m_rebuild_list.end();
       iterator++) {
    Chunk *chunk = (*iterator);
    if (chunk->is_loaded() && chunk->is_setup()) {
      chunk->update(0);
      m_flags_list.push_back(chunk);
    }
  }
}

void ChunkManager::update_unload_list() {
  ChunkList::iterator iterator;
  for (iterator = m_unload_list.begin(); iterator != m_unload_list.end();
       iterator++) {
    Chunk *chunk = (*iterator);
    if (chunk->is_loaded()) {
      chunk->unload();
    }
  }

  m_unload_list.clear();
}

// TODO: Implement the methods below.

void ChunkManager::update_render_list() {}

void ChunkManager::update_flags_list() {}
