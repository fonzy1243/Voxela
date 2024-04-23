#ifndef COMPUTE_SHADER_H
#define COMPUTE_SHADER_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class ComputeShader {
public:
  unsigned int ID;
  ComputeShader(const char *compute_path) {
    std::string shader_code;
    std::ifstream shader_file;

    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
      shader_file.open(compute_path);
      std::stringstream shader_stream;
      shader_stream << shader_file.rdbuf();
      shader_file.close();
      shader_code = shader_stream.str();
    } catch (std::ifstream::failure e) {
      std::cout << "ERROR::COMPUTE_SHADER::FILE_NOT_SUCCESSFULLY_READ"
                << std::endl;
    }

    const char *c_shader_code = shader_code.c_str();
    int success;
    char info_log[512];

    unsigned int compute;

    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &c_shader_code, NULL);
    glCompileShader(compute);
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(compute, 512, NULL, info_log);
      std::cout << "ERROR:SHADER::COMPUTE::COMPILATION_FAILED\n"
                << info_log << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success) {
      glGetProgramInfoLog(ID, 512, NULL, info_log);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << info_log << std::endl;
    }

    glDeleteShader(compute);
  }

  void use() const { glUseProgram(ID); }

  void set_int(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  }

  void set_float(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }

  void set_vec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }

  void set_vec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }

  void set_vec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
  }
};

#endif // !COMPUTE_SHADER_H
