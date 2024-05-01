#pragma once

#include "rendering/Mesh.hpp"

class MeshGenerator {
public:
  enum class MeshType {
    QUAD,
    PLANE,
    TETRAHEDRON,
    ICOSAHEDRON,
    CUBE,
    BUNNY,
    BUNNY_REDUCED
  };

  static Mesh generateQuad();
  static Mesh generatePlane(int sectorCount = 10, int stackCount = 10);
  static Mesh generateTetrahedron();
  static Mesh generateIcosahedron();
  static Mesh generateCube();
  static Mesh generateSphere(int sectorCount, int stackCount);
};
