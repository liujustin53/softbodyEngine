#pragma once

#include "core/Object.hpp"
#include "rendering/Mesh.hpp"

class MeshObject : public Object {
public:
  MeshObject(const Mesh &mesh, const glm::vec3 &color = glm::vec3(1.0f));
  MeshObject(const std::string &filename);

protected:
  virtual unsigned int indicesCount() const override {
    return _mesh._indices.size();
  }

private:
  Mesh _mesh;
};
