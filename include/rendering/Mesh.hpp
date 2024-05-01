#pragma once

#include <vector>

#include "rendering/MeshVertex.hpp"

struct Mesh {
  // public:
  //   Mesh() = default;
  //   Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
  //       : _vertices(vertices), _indices(indices) {}
  //
  // private:
  std::vector<MeshVertex> _vertices;
  std::vector<unsigned int> _indices;
};
