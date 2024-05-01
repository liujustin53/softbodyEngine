#include "core/AABB.hpp"

#include "core/Ray.hpp"

std::vector<glm::vec3> AABB::vertices() const {
  std::vector<glm::vec3> vertices;
  vertices.reserve(8);

  vertices.push_back(min);
  vertices.push_back({min.x, min.y, max.z});
  vertices.push_back({min.x, max.y, min.z});
  vertices.push_back({min.x, max.y, max.z});
  vertices.push_back({max.x, min.y, min.z});
  vertices.push_back({max.x, min.y, max.z});
  vertices.push_back({max.x, max.y, min.z});
  vertices.push_back(max);

  return vertices;
}

bool AABB::intersects(const AABB &other, const glm::mat4 &otherModelMatrix,
                      const glm::mat4 &thisModelMatrix) const {
  glm::vec3 worldMin = thisModelMatrix * glm::vec4(min, 1.0f);
  glm::vec3 worldMax = thisModelMatrix * glm::vec4(max, 1.0f);
  glm::vec3 otherWorldMin = otherModelMatrix * glm::vec4(other.min, 1.0f);
  glm::vec3 otherWorldMax = otherModelMatrix * glm::vec4(other.max, 1.0f);

  return worldMin.x <= otherWorldMax.x && worldMax.x >= otherWorldMin.x &&
         worldMin.y <= otherWorldMax.y && worldMax.y >= otherWorldMin.y &&
         worldMin.z <= otherWorldMax.z && worldMax.z >= otherWorldMin.z;
}

float AABB::intersects(const Ray &ray, const glm::mat4 &modelMatrix) const {
  // https://tavianator.com/2011/ray_box.html
  glm::vec3 worldMin = modelMatrix * glm::vec4(min, 1.0f);
  glm::vec3 worldMax = modelMatrix * glm::vec4(max, 1.0f);

  float tMin = std::numeric_limits<float>::min();
  float tMax = std::numeric_limits<float>::max();

  for (int i = 0; i < 3; i++) {
    float t1 = (worldMin[i] - ray.origin[i]) * ray.invDir[i];
    float t2 = (worldMax[i] - ray.origin[i]) * ray.invDir[i];

    tMin = std::max(tMin, std::min(t1, t2));
    tMax = std::min(tMax, std::max(t1, t2));
  }

  if (tMax >= tMin && tMax >= 0.0f) {
    return tMin;
  }

  if (tMin < 0.0f) {
    return tMax;
  }

  return -1.0f;
}
