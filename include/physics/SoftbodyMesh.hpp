#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "rendering/Mesh.hpp"

struct PointMass {
  glm::vec3 position{0.0f};
  glm::vec3 prevPosition{0.0f};
  glm::vec3 velocity{0.0f};
  float invMass{0.0f};

  glm::vec2 uv{-1.0f};    // For rendering
  glm::vec3 normal{0.0f}; // For rendering
};

struct SoftbodyEdge {
  unsigned int pointMassIndices[2];
  unsigned int faceIndices[2];
  unsigned int neighborIndices[2]; // Indices of the two points on the faces
                                   // that are not part of the edge

  float restLength{0.0f};
  float restSpanLength{0.0f}; // Length between the two faces
  // float restAngle{0.0};

  float lambdaLength{0.0f};
  float lambdaSpanLength{0.0f};
  // float lambdaAngle{0.0f};
};

struct SoftbodyFace {
  unsigned int pointMassIndices[3];
};

struct SoftbodyMesh {
  SoftbodyMesh() = default;
  SoftbodyMesh(const Mesh &mesh);

  float calculateVolume() const;
  glm::vec3 getCenter() const;

  std::vector<PointMass> pointMasses;
  std::vector<SoftbodyEdge> edges;
  std::vector<SoftbodyFace> faces;

  float restVolume{0.0f};
  float lambdaVolume{0.0f};

  // Inverse stiffnesses (1/k)
  float distanceCompliance{0.001f};
  float volumeCompliance{0.0f};
  float bendingCompliance{0.005f};

  float pressure{1.0f};
};
