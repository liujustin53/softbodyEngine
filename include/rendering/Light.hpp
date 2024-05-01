#pragma once

#include <glm/vec3.hpp>

#include "core/Entity.hpp"

class Light : public Entity {
public:
  Light();

  void update(float deltaTime) override;

  glm::vec3 color{1.0f, 1.0f, 1.0f};

  float ambient{0.2f};
  float diffuse{0.5f};
  float specular{0.5f};

  float constant{1.0f};
  float linear{0.09f};
  float quadratic{0.032f};

  // For shadow mapping
  glm::mat4 lightProjection, lightView, lightSpaceMatrix;
};
