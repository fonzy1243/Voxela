#ifndef BLOCK_H
#define BLOCK_H

class Block {
public:
  Block();
  bool is_active();
  void set_active(bool active);

private:
  bool m_is_active;
};

#endif
