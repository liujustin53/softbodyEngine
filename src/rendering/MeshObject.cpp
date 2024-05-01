#include "rendering/MeshObject.hpp"

#include "core/ObjLoader.hpp"

MeshObject::MeshObject(const Mesh &mesh, const glm::vec3 &color)
    : Object(color), _mesh(mesh) {
  _vertexBufferLayout.createBufferLayout(_mesh._vertices, _mesh._indices);
}

MeshObject::MeshObject(const std::string &filename) {
  std::vector<Texture> textures;

  ObjLoader::loadMesh(filename, _mesh, textures);

  // Load textures
  addTextures(textures);

  _vertexBufferLayout.createBufferLayout(_mesh._vertices, _mesh._indices);
}
