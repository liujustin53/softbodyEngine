#include "rendering/Shader.hpp"

#include "core/util.hpp"
#include "rendering/Light.hpp"

#include <iostream>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  load(vertexPath, fragmentPath);
}

void Shader::load(const std::string &vertexPath,
                  const std::string &fragmentPath) {
  std::string vertexShaderSource = loadShaderAsString(vertexPath);
  std::string fragmentShaderSource = loadShaderAsString(fragmentPath);

  id = createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void Shader::use() const { glUseProgram(id); }

void Shader::setBool(const std::string &name, bool value) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniform1i(loc, (int)value);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setInt(const std::string &name, int value) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniform1i(loc, value);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setFloat(const std::string &name, float value) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniform1f(loc, value);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniform2fv(loc, 1, &value[0]);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setVec2(const std::string &name, float x, float y) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniform2f(loc, x, y);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniform3fv(loc, 1, &value[0]);
  } else {
    // std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniform3f(loc, x, y, z);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniform4fv(loc, 1, &value[0]);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setVec4(const std::string &name, float x, float y, float z,
                     float w) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniform4f(loc, x, y, z, w);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniformMatrix2fv(loc, 1, GL_FALSE, &mat[0][0]);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
  GLint loc = glGetUniformLocation(id, name.c_str());
  if (loc >= 0) {
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
  } else {
    std::cout << "Could not find " << name << ", maybe a mispelling?\n";
  }
}

void Shader::setLight(const std::string &name, const Light &light) const {
  setVec3("u_PointLight.position", light.getTransform().getPosition());
  setVec3("u_PointLight.color", light.color);
  setFloat("u_PointLight.ambient", light.ambient);
  setFloat("u_PointLight.diffuse", light.diffuse);
  setFloat("u_PointLight.specular", light.specular);
  setFloat("u_PointLight.constant", light.constant);
  setFloat("u_PointLight.linear", light.linear);
  setFloat("u_PointLight.quadratic", light.quadratic);
}
