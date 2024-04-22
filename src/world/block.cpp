#include <block.h>

Block::Block() { m_is_active = false; }

bool Block::is_active() { return m_is_active; }

void Block::set_active(bool active) { m_is_active = active; }
