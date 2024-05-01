#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include <glm/glm.hpp>

#include "core/ObjLoader.hpp"

#include "rendering/MeshVertex.hpp"
#include "rendering/Texture.hpp"

bool ObjLoader::loadMesh(const std::string &filename, Mesh &out_mesh,
                         std::vector<Texture> &out_textures) {
  std::vector<glm::vec3> temp_vertices;
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;

  if (!ObjLoader::loadObj(filename, temp_vertices, temp_uvs, temp_normals,
                          out_textures)) {
    throw std::runtime_error("Failed to load obj file");
  }

  std::vector<glm::vec3> tangents;
  std::vector<glm::vec3> bitangents;

  if (!ObjLoader::computeTangents(temp_vertices, temp_uvs, temp_normals,
                                  tangents, bitangents)) {
    throw std::runtime_error("Failed to compute tangents");
  }

  if (!ObjLoader::indexVBO(temp_vertices, temp_uvs, temp_normals, tangents,
                           bitangents, out_mesh._vertices, out_mesh._indices)) {
    throw std::runtime_error("Failed to create index buffer");
  }

  return true;
}

bool ObjLoader::loadObj(const std::string &filename,
                        std::vector<glm::vec3> &out_vertices,
                        std::vector<glm::vec2> &out_uvs,
                        std::vector<glm::vec3> &out_normals,
                        std::vector<Texture> &out_textures) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::string error = "Failed to open file: " + filename;
    throw std::runtime_error(error);
  }

  std::vector<glm::vec3> temp_vertices;
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;
  std::vector<ObjFace> temp_faces;

  // file format:
  // v x y z
  // vn nx ny nz
  // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
  // vt and vn are optional

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream stream(line);
    std::string token;
    stream >> token;

    // read for mtllib
    if (token == "mtllib") {
      std::string mtlFilename;
      stream >> mtlFilename;
      // Get full path
      mtlFilename =
          filename.substr(0, filename.find_last_of("/")) + "/" + mtlFilename;
      loadMtl(mtlFilename, out_textures);
      continue;
    }

    switch (token[0]) {
    case 'v': {
      if (token.size() == 1) {
        float x, y, z;
        stream >> x >> y >> z;
        temp_vertices.push_back({x, y, z});
      } else if (token[1] == 't') {
        float u, v;
        stream >> u >> v;
        temp_uvs.push_back({u, v});
      } else if (token[1] == 'n') {
        float x, y, z;
        stream >> x >> y >> z;
        temp_normals.push_back({x, y, z});
      }
      break;
    }
    case 'f': {
      // handle optional vt and vn
      unsigned int vertexIndex[3] = {0, 0, 0}, textureIndex[3] = {0, 0, 0},
                   normalIndex[3] = {0, 0, 0};
      for (int i = 0; i < 3; i++) {
        std::string vertex;
        stream >> vertex;
        std::istringstream vertexStream(vertex);
        vertexStream >> vertexIndex[i];
        vertexStream.ignore(1);
        if (vertexStream.peek() != '/') {
          vertexStream >> textureIndex[i];
        }
        vertexStream.ignore(1);
        vertexStream >> normalIndex[i];
      }
      temp_faces.push_back(
          {{vertexIndex[0] - 1, vertexIndex[1] - 1, vertexIndex[2] - 1},
           {textureIndex[0] - 1, textureIndex[1] - 1, textureIndex[2] - 1},
           {normalIndex[0] - 1, normalIndex[1] - 1, normalIndex[2] - 1}});
      break;
    }
    default:
      break;
    }
  }

  // Create 3 vertices for each face
  for (const auto &face : temp_faces) {
    for (int i = 0; i < 3; i++) {
      unsigned int vertexIndex = face.vertexIndices[i];
      unsigned int uvIndex = face.uvIndices[i];
      unsigned int normalIndex = face.normalIndices[i];

      glm::vec3 vertex =
          vertexIndex != -1 ? temp_vertices[vertexIndex] : glm::vec3(0, 0, 0);
      glm::vec2 uv = uvIndex != -1 ? temp_uvs[uvIndex] : glm::vec2(-1, -1);
      glm::vec3 normal =
          normalIndex != -1 ? temp_normals[normalIndex] : glm::vec3(0, 0, 0);
      out_vertices.push_back(vertex);
      out_uvs.push_back(uv);
      out_normals.push_back(normal);
    }
  }

  return true;
}

bool ObjLoader::computeNormals(const std::vector<glm::vec3> &in_vertices,
                               const std::vector<unsigned int> &in_indices,
                               std::vector<glm::vec3> &out_normals) {
  out_normals.resize(in_vertices.size(), glm::vec3(0, 0, 0));

  for (size_t i = 0; i < in_indices.size(); i += 3) {
    // Shortcuts for vertices
    const glm::vec3 &a = in_vertices[in_indices[i + 0]];
    const glm::vec3 &b = in_vertices[in_indices[i + 1]];
    const glm::vec3 &c = in_vertices[in_indices[i + 2]];

    // Edges of the triangle
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;

    // Normal of the triangle
    glm::vec3 normal = glm::cross(ab, ac);

    // Set the same normal for all three vertices of the triangle.
    out_normals[in_indices[i + 0]] += normal;
    out_normals[in_indices[i + 1]] += normal;
    out_normals[in_indices[i + 2]] += normal;
  }

  for (size_t i = 0; i < out_normals.size(); i++) {
    out_normals[i] = glm::normalize(out_normals[i]);
  }

  return true;
}

bool ObjLoader::computeTangents(const std::vector<glm::vec3> &in_vertices,
                                const std::vector<glm::vec2> &in_uvs,
                                const std::vector<glm::vec3> &in_normals,
                                std::vector<glm::vec3> &out_tangents,
                                std::vector<glm::vec3> &out_bitangents) {
  for (size_t i = 0; i < in_vertices.size(); i += 3) {
    // Shortcuts for vertices
    const glm::vec3 &v0 = in_vertices[i + 0];
    const glm::vec3 &v1 = in_vertices[i + 1];
    const glm::vec3 &v2 = in_vertices[i + 2];

    // Shortcuts for UVs
    const glm::vec2 &uv0 = in_uvs[i + 0];
    const glm::vec2 &uv1 = in_uvs[i + 1];
    const glm::vec2 &uv2 = in_uvs[i + 2];

    // Edges of the triangle : position delta
    glm::vec3 deltaPos1 = v1 - v0;
    glm::vec3 deltaPos2 = v2 - v0;

    // UV delta
    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

    // Set the same tangent for all three vertices of the triangle.
    // They will be merged later, in indexVBO
    out_tangents.push_back(tangent);
    out_tangents.push_back(tangent);
    out_tangents.push_back(tangent);

    // Same thing for bitangents
    out_bitangents.push_back(bitangent);
    out_bitangents.push_back(bitangent);
    out_bitangents.push_back(bitangent);
  }

  return true;
}

bool ObjLoader::indexVBO(const std::vector<glm::vec3> &in_vertices,
                         const std::vector<glm::vec2> &in_uvs,
                         const std::vector<glm::vec3> &in_normals,
                         const std::vector<glm::vec3> &in_tangents,
                         const std::vector<glm::vec3> &in_bitangents,
                         std::vector<MeshVertex> &out_vertices,
                         std::vector<unsigned int> &out_indices) {
  std::map<ObjVertex, unsigned int> vertexToIndex;
  for (int i = 0; i < in_vertices.size(); i++) {
    ObjVertex objVert = {in_vertices[i], in_uvs[i], in_normals[i]};

    auto it = vertexToIndex.find(objVert);
    if (it == vertexToIndex.end()) {
      // Not found, add a new vertex
      unsigned int index = out_vertices.size();
      vertexToIndex[objVert] = index;
      out_indices.push_back(index);
      out_vertices.push_back({in_vertices[i], in_uvs[i], in_normals[i],
                              in_tangents[i], in_bitangents[i]});
    } else {
      // Found, use the existing vertex
      int index = it->second;
      out_indices.push_back(index);

      // Update the tangent and bitangent
      out_vertices[index].tangent += in_tangents[i];
      out_vertices[index].bitangent += in_bitangents[i];
    }
  }

  return true;
}

bool ObjLoader::loadMtl(const std::string &filename,
                        std::vector<Texture> &out_textures) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file");
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream stream(line);
    std::string token;
    stream >> token;
    if (token == "map_Kd" || token == "map_Bump" || token == "map_Ks") {
      std::string texturePath;
      stream >> texturePath;
      // Get full path
      texturePath =
          filename.substr(0, filename.find_last_of("/")) + "/" + texturePath;
      Texture::TextureType type;
      if (token == "map_Kd") {
        type = Texture::TextureType::DIFFUSE;
      } else if (token == "map_Ks") {
        type = Texture::TextureType::SPECULAR;
      } else if (token == "map_Bump") {
        type = Texture::TextureType::NORMAL;
      }
      out_textures.push_back(Texture(texturePath, type));
    }
  }

  return true;
}
