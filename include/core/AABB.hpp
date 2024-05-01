#pragma once

#include <glm/vec3.hpp>

#include <vector>

#include <glm/mat4x4.hpp>

struct Ray;

class Transform;

struct AABB {
  glm::vec3 min{0.0f};
  glm::vec3 max{0.0f};

  std::vector<glm::vec3> vertices() const;

  // Returns true if the AABB intersects with the other AABB
  bool intersects(const AABB &other, const glm::mat4 &otherModelMatrix,
                  const glm::mat4 &thisModelMatrix) const;
  // Returns the t distance along the ray where the intersection occurs
  float intersects(const Ray &ray, const glm::mat4 &modelMatrix) const;
};
