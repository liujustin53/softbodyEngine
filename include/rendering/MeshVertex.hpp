#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct MeshVertex {
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::vec2 uv{-1.0f, -1.0f};
  glm::vec3 normal{0.0f, 0.0f, 0.0f};
  glm::vec3 tangent{0.0f, 0.0f, 0.0f};
  glm::vec3 bitangent{0.0f, 0.0f, 0.0f};
};
