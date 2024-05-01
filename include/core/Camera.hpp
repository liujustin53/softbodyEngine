
/** @file Camera.hpp
 *  @brief Sets up an OpenGL camera.
 *
 *  Sets up an OpenGL Camera. The camera is what
 *  sets up our 'view' matrix.
 *
 *  @author Mike
 *  @bug No known bugs.
 */
#pragma once

#include "glm/glm.hpp"

#include "Transform.hpp"

class Camera {
public:
  // Constructor to create a camera
  Camera(int windowWidth, int windowHeight, float fov = 90.0f, float nearPlane = 0.1f,
         float farPlane = 1000.0f);

  // Return a 'view' matrix with our camera transformation applied.
  glm::mat4 getViewMatrix() const;
  // Return the projection matrix for our camera
  const glm::mat4 &getProjectionMatrix() const { return _projectionMatrix; }

  // Move the camera around
  void mouseLook(int mouseX, int mouseY);
  void moveForward(float speed);
  void moveBackward(float speed);
  void moveLeft(float speed);
  void moveRight(float speed);
  void moveUp(float speed);
  void moveDown(float speed);

  // Look at a target
  void lookAt(const glm::vec3 &target);

  // Get the position of the camera
  Transform &getTransform();
  const Transform &getTransform() const;

private:
  // Track the old mouse position
  glm::vec2 _oldMousePosition;
  // Where is our camera positioned
  Transform _transform;
  // What direction is the camera looking
  glm::vec3 _viewDirection;
  // Which direction is 'up' in our world
  glm::vec3 _upVector;
  // The projection matrix for our camera
  glm::mat4 _projectionMatrix;
};
