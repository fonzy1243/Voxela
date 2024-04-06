#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
  unsigned int ID;

  Shader(const char *vertex_path, const char *fragment_path) {
    std::string vertex_code;
    std::string fragment_code;
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;

    v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
      v_shader_file.open(vertex_path);
      f_shader_file.open(fragment_path);
      std::stringstream v_shader_stream, f_shader_stream;

      v_shader_stream << v_shader_file.rdbuf();
      f_shader_stream << f_shader_file.rdbuf();

      v_shader_file.close();
      f_shader_file.close();

      vertex_code = v_shader_stream.str();
      fragment_code = f_shader_stream.str();
    } catch (std::ifstream::failure e) {
      std::cout << "ERROR::SHADDER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char *v_shader_code = vertex_code.c_str();
    const char *f_shader_code = fragment_code.c_str();

    unsigned int vertex, fragment;
    int success;
    char info_log[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(vertex, 512, NULL, info_log);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << info_log << std::endl;
    };

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, NULL);
    glCompileShader(fragment);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(fragment, 512, NULL, info_log);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << info_log << std::endl;
    };

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success) {
      glGetProgramInfoLog(ID, 512, NULL, info_log);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << info_log << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }

  void use() { glUseProgram(ID); }

  void set_bool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }

  void set_int(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }

  void set_float(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }
};

#endif
