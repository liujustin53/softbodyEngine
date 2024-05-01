#include "rendering/VertexBufferLayout.hpp"

#include <glad/glad.h>

#include "rendering/MeshVertex.hpp"

VertexBufferLayout::~VertexBufferLayout() {
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
}

void VertexBufferLayout::bind() const {
  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
}

void VertexBufferLayout::unbind() const {
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VertexBufferLayout::createBufferLayout(
    std::vector<MeshVertex> &vertices, std::vector<unsigned int> &indices) {
  // Set up VAO, VBO, EBO
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex),
               vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                        (void *)offsetof(MeshVertex, position));

  // uv
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                        (void *)offsetof(MeshVertex, uv));

  // normal
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                        (void *)offsetof(MeshVertex, normal));

  // tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                        (void *)offsetof(MeshVertex, tangent));

  // bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                        (void *)offsetof(MeshVertex, bitangent));

  glBindVertexArray(0);
}

void VertexBufferLayout::createSoftBodyBufferLayout(
    std::vector<PointMass> &vertices, std::vector<SoftbodyFace> &faces) {
  // Set up VAO, VBO, EBO
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PointMass),
               vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(SoftbodyFace) * 3,
               faces.data(), GL_STATIC_DRAW);

  // position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PointMass),
                        (void *)offsetof(PointMass, position));

  // uv
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(PointMass),
                        (void *)offsetof(PointMass, uv));

  // normal
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(PointMass),
                        (void *)offsetof(PointMass, normal));

  glBindVertexArray(0);
}

void VertexBufferLayout::updateSoftBodyBufferLayout(
    std::vector<PointMass> &vertices, std::vector<SoftbodyFace> &faces) {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PointMass),
               vertices.data(), GL_STATIC_DRAW);
}
