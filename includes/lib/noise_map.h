#ifndef NOISE_MAP_H
#define NOISE_MAP_H

#include <compute_shader.h>
#include <vector>

const int NOISE_HEIGHT = 2048;
const float NOISE_SCALE = 187.45;

class Noise {
public:
  Noise(int seed);
  int get_dim();
  std::vector<float> height;

private:
  int dim = NOISE_HEIGHT;
  int dim2 = NOISE_HEIGHT * NOISE_HEIGHT;
  float scale = NOISE_SCALE;
  ComputeShader &shader;
};

#endif
