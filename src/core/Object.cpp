#include "core/Object.hpp"

#include "core/ObjLoader.hpp"

#include "rendering/Shader.hpp"

void Object::draw(const Shader &shader) const {
  // Bind
  for (unsigned int i = 0; i < _textures.size(); i++) {
    _textures[i].bind(shader, i + 1);
  }
  _vertexBufferLayout.bind();

  // Draw
  shader.setVec3("u_VertexColor", _color);
  glDrawElements(GL_TRIANGLES, indicesCount(), GL_UNSIGNED_INT, 0);

  // Unbind
  // Might not be necessary, but it's good practice
  _vertexBufferLayout.unbind();
  for (const auto &texture : _textures) {
    texture.unbind();
  }
}
