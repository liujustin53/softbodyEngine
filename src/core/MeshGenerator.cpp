#include "core/MeshGenerator.hpp"

#include "core/ObjLoader.hpp"

#include <cmath>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

Mesh MeshGenerator::generateQuad() {
  std::vector<MeshVertex> meshVertices;
  std::vector<unsigned int> indices;

  meshVertices.push_back({glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(-1.0f, -1.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f)});
  meshVertices.push_back({glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(-1.0f, 1.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f)});
  meshVertices.push_back({glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, -1.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f)});
  meshVertices.push_back({glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f)});
  indices = {0, 1, 2, 2, 1, 3};

  return {meshVertices, indices};
}

Mesh MeshGenerator::generatePlane(int sectorCount, int stackCount) {
  std::vector<MeshVertex> meshVertices;
  std::vector<unsigned int> indices;

  float sectorStep = 1.0f / sectorCount;
  float stackStep = 1.0f / stackCount;

  for (int i = 0; i <= stackCount; i++) {
    float stack = i * stackStep - 0.5f;
    for (int j = 0; j <= sectorCount; j++) {
      float sector = j * sectorStep - 0.5f;
      MeshVertex v;
      v.position = glm::vec3(sector, 0.0f, stack);
      v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
      meshVertices.push_back(v);
    }
  }

  for (int i = 0; i < stackCount; i++) {
    for (int j = 0; j < sectorCount; j++) {
      int k1 = i * (sectorCount + 1) + j;
      int k2 = k1 + 1;
      int k3 = (i + 1) * (sectorCount + 1) + j;
      int k4 = k3 + 1;
      indices.push_back(k1);
      indices.push_back(k3);
      indices.push_back(k2);
      indices.push_back(k2);
      indices.push_back(k3);
      indices.push_back(k4);
    }
  }

  return {meshVertices, indices};
}

Mesh MeshGenerator::generateTetrahedron() {
  float a = 1.0f / 3.0f;
  float b = std::sqrt(8.0f) / 3.0f;
  float c = std::sqrt(2.0f) / 3.0f;
  float d = std::sqrt(2.0f / 3.0f);

  std::vector<glm::vec3> vertices = {
      glm::vec3(0, 1, 0),
      glm::vec3(-c, -a, d),
      glm::vec3(-c, -a, -d),
      glm::vec3(b, -a, 0),
  };

  std::vector<unsigned int> indices = {
      0, 1, 2, 0, 2, 3, 0, 3, 1, 1, 3, 2,
  };

  std::vector<glm::vec3> normals;
  ObjLoader::computeNormals(vertices, indices, normals);

  std::vector<MeshVertex> meshVertices;
  for (size_t i = 0; i < vertices.size(); i++) {
    MeshVertex v;
    v.position = vertices[i];
    v.normal = normals[i];
    meshVertices.push_back(v);
  }

  return {meshVertices, indices};
}

Mesh MeshGenerator::generateIcosahedron() {
  float phi = (1.0f + std::sqrt(5.0f)) / 2.0f;
  float a = 1.0f;
  float b = 1.0f / phi;

  std::vector<glm::vec3> vertices = {
      glm::vec3(0, b, -a),  glm::vec3(b, a, 0),  glm::vec3(-b, a, 0),
      glm::vec3(0, b, a),   glm::vec3(0, -b, a), glm::vec3(-a, 0, b),
      glm::vec3(0, -b, -a), glm::vec3(a, 0, -b), glm::vec3(a, 0, b),
      glm::vec3(-a, 0, -b), glm::vec3(b, -a, 0), glm::vec3(-b, -a, 0)};

  std::vector<unsigned int> indices = {
      2, 1,  0,  1, 2, 3, 5, 4, 3,  4,  8, 3, 7,  6, 0, 6,  9, 0,  11, 10,
      4, 10, 11, 6, 9, 5, 2, 5, 9,  11, 8, 7, 1,  7, 8, 10, 2, 5,  3,  8,
      1, 3,  9,  2, 0, 1, 7, 0, 11, 9,  6, 7, 10, 6, 5, 11, 4, 10, 8,  4};

  std::vector<glm::vec3> normals;
  ObjLoader::computeNormals(vertices, indices, normals);

  std::vector<MeshVertex> meshVertices;
  for (size_t i = 0; i < vertices.size(); i++) {
    MeshVertex v;
    v.position = vertices[i];
    v.normal = normals[i];
    meshVertices.push_back(v);
  }

  return {meshVertices, indices};
}

Mesh MeshGenerator::generateCube() {
  std::vector<glm::vec3> vertices = {
      // front
      glm::vec3(-0.5f, -0.5f, 0.5f),
      glm::vec3(0.5f, -0.5f, 0.5f),
      glm::vec3(0.5f, 0.5f, 0.5f),
      glm::vec3(-0.5f, 0.5f, 0.5f),
      // back
      glm::vec3(-0.5f, -0.5f, -0.5f),
      glm::vec3(0.5f, -0.5f, -0.5f),
      glm::vec3(0.5f, 0.5f, -0.5f),
      glm::vec3(-0.5f, 0.5f, -0.5f),
  };

  // clang-format off
  std::vector<unsigned int> indices = {
      // front
      0, 1, 2, 2, 3, 0,
      // right
      1, 5, 6, 6, 2, 1,
      // back
      7, 6, 5, 5, 4, 7,
      // left
      4, 0, 3, 3, 7, 4,
      // bottom
      4, 5, 1, 1, 0, 4,
      // top
      3, 2, 6, 6, 7, 3,
  };

  std::vector<glm::vec3> normals;
  ObjLoader::computeNormals(vertices, indices, normals);

  std::vector<MeshVertex> meshVertices;
  for (size_t i = 0; i < vertices.size(); i++) {
    MeshVertex v;
    v.position = vertices[i];
    v.normal = normals[i];
    meshVertices.push_back(v);
  }

  return {meshVertices, indices};
}
