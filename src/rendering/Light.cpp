#include "rendering/Light.hpp"

#include "core/MeshGenerator.hpp"

Light::Light() {
  // Create a cube mesh for the light
  setObject(MeshGenerator::generateCube());
  getObject()->setStatic(true);

  _transform.setScale(0.2f);

  lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 100.0f);
  lightView = glm::lookAt(_transform.getPosition(), glm::vec3(0.0f),
                          glm::vec3(0.0f, 1.0f, 0.0f));
  lightSpaceMatrix = lightProjection * lightView;
}

void Light::update(float deltaTime) {
  // Update the light's position
  // static float increment = 0.00f;
  // increment += 1.0f * deltaTime;
  // if (increment > 2 * M_PI) {
  //   increment = 0.0f;
  // }
  //
  // static float radius = 8.0f;
  //
  // _transform.setPosition(cos(increment) * radius, _transform.getPosition().y,
  //                        sin(increment) * radius);

  lightView = glm::lookAt(_transform.getPosition(), glm::vec3(0.0f),
                          glm::vec3(0.0f, 1.0f, 0.0f));
  lightSpaceMatrix = lightProjection * lightView;

  Entity::update(deltaTime);
}
