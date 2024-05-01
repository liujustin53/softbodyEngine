#pragma once

#include "rendering/Mesh.hpp"

#include <cstring>
#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct MeshVertex;
class Texture;

class ObjLoader {
public:
  /**
   * Load the mesh at the given path
   * @param filename the path to the mesh file
   * @return the loaded mesh
   */
  static bool loadMesh(const std::string &filename, Mesh &out_mesh,
                       std::vector<Texture> &out_textures);

  /**
   * Load the obj file at the given path into the given vector of vertices
   * @param filename the path to the obj file
   * @param out_vertices the vector to load the vertices into
   * @param out_uvs the vector to load the uvs into
   * @param out_normals the vector to load the normals into
   * @return true if the file was loaded successfully, false otherwise
   */
  static bool loadObj(const std::string &filename,
                      std::vector<glm::vec3> &out_vertices,
                      std::vector<glm::vec2> &out_uvs,
                      std::vector<glm::vec3> &out_normals,
                      std::vector<Texture> &out_textures);

  /**
   * Compute the normals for the given vertices and indices.
   * Used to compute normals for a generated mesh.
   * @param in_vertices the vertices
   * @param in_indices the indices
   * @param out_normals the final normal data
   * @return true if the normals were computed successfully, false otherwise
   */
  static bool computeNormals(const std::vector<glm::vec3> &in_vertices,
                             const std::vector<unsigned int> &in_indices,
                             std::vector<glm::vec3> &out_normals);

  /**
   * Compute the tangents for the given vertices, uvs, and normals
   * @param in_vertices the vertices
   * @param in_uvs the uvs
   * @param in_normals the normals
   * @param out_vertices the final vertex data
   * @return true if the tangents were computed successfully, false otherwise
   */
  static bool computeTangents(const std::vector<glm::vec3> &in_vertices,
                              const std::vector<glm::vec2> &in_uvs,
                              const std::vector<glm::vec3> &in_normals,
                              std::vector<glm::vec3> &out_tangents,
                              std::vector<glm::vec3> &out_bitangents);

  /**
   * Create a single index buffer from the given vertex, uv, and normal data
   * @param in_vertices the vertices
   * @param in_uvs      the uvs
   * @param in_normals  the normals
   * @param out_vertices the final vertex data
   * @param out_indices  the final index data
   * @return true if the index buffer was created successfully, false
   * otherwise
   */
  static bool indexVBO(const std::vector<glm::vec3> &in_vertices,
                       const std::vector<glm::vec2> &in_uvs,
                       const std::vector<glm::vec3> &in_normals,
                       const std::vector<glm::vec3> &in_tangents,
                       const std::vector<glm::vec3> &in_bitangents,
                       std::vector<MeshVertex> &out_vertices,
                       std::vector<unsigned int> &out_indices);

  /**
   * Load the mtl file at the given path into the given vector of textures
   * @param filename the path to the mtl file
   * @param out_textures the vector to load the textures into
   * @return true if the file was loaded successfully, false otherwise
   */
  static bool loadMtl(const std::string &filename,
                      std::vector<Texture> &out_textures);

private:
  // intermediate structures for loading obj files
  struct ObjVertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator<(const ObjVertex that) const {
      return memcmp((void *)this, (void *)&that, sizeof(ObjVertex)) > 0;
    };
  };
  struct ObjFace {
    unsigned int vertexIndices[3];
    unsigned int uvIndices[3];
    unsigned int normalIndices[3];
  };
};
