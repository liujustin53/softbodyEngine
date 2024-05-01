#pragma once

#include "rendering/Texture.hpp"
#include "rendering/VertexBufferLayout.hpp"

#include <glm/glm.hpp>

class Shader;
class Transform;

class Object {
public:
  Object(const glm::vec3 &color = glm::vec3(1.0f)) : _color(color){};

  virtual void update(float deltaTime, Transform &transform){};
  virtual void draw(const Shader &shader) const;

  void addTexture(const Texture &texture) {
    _textures.push_back(texture);
    _textures.back().load();
  }

  void addTextures(const std::vector<Texture> &textures) {
    int oldSize = _textures.size();
    _textures.insert(_textures.end(), textures.begin(), textures.end());
    for (int i = oldSize; i < _textures.size(); i++) {
      _textures[i].load();
    }
  }

  void setColor(const glm::vec3 &color) { _color = color; }

protected:
  VertexBufferLayout _vertexBufferLayout;

  virtual unsigned int indicesCount() const = 0;

private:
  std::vector<Texture> _textures;
  // Default color if no textures are loaded
  glm::vec3 _color{0.55f, 0.55f, 0.55f};
};
