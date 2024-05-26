#pragma once

#include <glad/glad.h>

#include <vector>

class StorageBuffer {
public:
  GLuint ssbo = 0;

  StorageBuffer() = default;
  ~StorageBuffer() { glDeleteBuffers(1, &ssbo); };

  void bind() const { glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo); };
  void unbind() const { glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); }

  template <typename T>
  void createStorageBuffer(const std::vector<T> &data, GLenum usage,
                           unsigned int bindingPoint) {
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(T), data.data(),
                 usage);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  template <typename T>
  void updateStorageBuffer(const std::vector<T> &data) const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(T),
                    data.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }
};
