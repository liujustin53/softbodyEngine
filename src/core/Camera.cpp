#include "core/Camera.hpp"

#include "glm/gtx/rotate_vector.hpp"

Camera::Camera(int windowWidth, int windowHeight, float fov, float nearPlane,
               float farPlane) {
  // Looking down along the z-axis initially.
  // Remember, this is negative because we are looking 'into' the scene.
  _viewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
  // For now--our upVector always points up along the y-axis
  _upVector = glm::vec3(0.0f, 1.0f, 0.0f);
  _projectionMatrix = glm::perspective(glm::radians(fov),
                                       (float)windowWidth / (float)windowHeight,
                                       nearPlane, farPlane);
}

void Camera::mouseLook(int mouseX, int mouseY) {
  // Record our new position as a vector
  glm::vec2 newMousePosition(mouseX, mouseY);
  static bool firstLook = true;
  // Note: we need to handle the 'first' mouse event
  // Note: We should also 'center' the mouse in our
  //       program when we execute it.
  if (true == firstLook) {
    firstLook = false;
    _oldMousePosition = newMousePosition;
  }
  // Detect how much the mouse has moved since
  // the last time
  glm::vec2 mouseDelta = _oldMousePosition - newMousePosition;
  mouseDelta.x *= 0.2f; // mouse sensitivity.
  mouseDelta.y *= 0.2f; // mouse sensitivity.
  // Update the view direction around the up vector
  _viewDirection =
      glm::rotate(_viewDirection, glm::radians(mouseDelta.x), _upVector);

  // Compute the rightVector
  glm::vec3 rightVector = glm::cross(_viewDirection, _upVector);
  _viewDirection =
      glm::rotate(_viewDirection, glm::radians(mouseDelta.y), rightVector);

  // Update our old position after we have made changes
  _oldMousePosition = newMousePosition;
}

void Camera::moveForward(float speed) {
  _transform.translate(_viewDirection * speed);
}

void Camera::moveBackward(float speed) {
  _transform.translate(-_viewDirection * speed);
}

void Camera::moveLeft(float speed) {
  // Compute the rightVector
  glm::vec3 rightVector = glm::cross(_viewDirection, _upVector);
  _transform.translate(-rightVector * speed);
}

void Camera::moveRight(float speed) {
  // Compute the rightVector
  glm::vec3 rightVector = glm::cross(_viewDirection, _upVector);
  _transform.translate(rightVector * speed);
}

void Camera::moveUp(float speed) { _transform.translate(_upVector * speed); }

void Camera::moveDown(float speed) { _transform.translate(-_upVector * speed); }

void Camera::lookAt(const glm::vec3 &target) {
  _viewDirection = glm::normalize(target - _transform.getPosition());
}

Transform &Camera::getTransform() { return _transform; }

const Transform &Camera::getTransform() const { return _transform; }

glm::mat4 Camera::getViewMatrix() const {
  // Think about the second argument and why that is
  // setup as it is.
  glm::vec3 position = _transform.getPosition();
  return glm::lookAt(position, position + _viewDirection, _upVector);
}
