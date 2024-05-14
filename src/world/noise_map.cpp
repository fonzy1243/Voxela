#include <compute_shader.h>
#include <gl_errors.h>
#include <limits>
#include <noise_map.h>
#include <vector>

extern ComputeShader *noise_shader;

Noise::Noise(int seed) : shader(*noise_shader) {
  height = std::vector<float>(dim2);
  struct MinMax {
    int min_height;
    int max_height;
  };

  MinMax data;
  data.max_height = std::numeric_limits<int>::min();
  data.min_height = std::numeric_limits<int>::max();

  shader.use();
  shader.set_int("dim", dim);
  shader.set_int("seed", seed);
  shader.set_float("scale", scale);

  unsigned int noise_ssbo, minmax_ssbo;

  glGenBuffers(1, &noise_ssbo);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, noise_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, noise_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, height.size() * sizeof(float),
               &height[0], GL_DYNAMIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glCheckError();

  glGenBuffers(1, &minmax_ssbo);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, minmax_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, minmax_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(MinMax), &data,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glCheckError();

  shader.use();
  glDispatchCompute(dim / 8, dim / 8, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  glCheckError();

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, noise_ssbo);
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, height.size() * sizeof(float),
                     &height[0]);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glCheckError();

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, minmax_ssbo);
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(MinMax), &data);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glCheckError();
}

int Noise::get_dim() { return dim; }
