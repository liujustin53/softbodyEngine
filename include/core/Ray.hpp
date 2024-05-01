#pragma once

#include <glm/vec3.hpp>

struct Ray {
  glm::vec3 origin{0.0f};
  glm::vec3 dir{0.0f};
  glm::vec3 invDir{0.0f};
  float t{-1.0f};
};
