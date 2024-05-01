#include "core/Transform.hpp"

void Transform::reset() {
  _position = glm::vec3(0.0f, 0.0f, 0.0f);
  _rotation = glm::vec3(0.0f, 0.0f, 0.0f);
  _scale = glm::vec3(1.0f, 1.0f, 1.0f);

  _modelMatrix = glm::mat4(1.0f);
}

// Math from learnopengl
glm::mat4 Transform::getLocalModelMatrix() const {
  const glm::mat4 rotationX = glm::rotate(
      glm::mat4(1.0f), glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  const glm::mat4 rotationY = glm::rotate(
      glm::mat4(1.0f), glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  const glm::mat4 rotationZ = glm::rotate(
      glm::mat4(1.0f), glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

  const glm::mat4 rotationMatrix = rotationX * rotationY * rotationZ;

  return glm::translate(glm::mat4(1.0f), _position) * rotationMatrix *
         glm::scale(glm::mat4(1.0f), _scale);
}

void Transform::computeModelMatrix(const glm::mat4 &parentModelMatrix) {
  _modelMatrix = parentModelMatrix * getLocalModelMatrix();
}

glm::mat4 Transform::getModelMatrix() const { return _modelMatrix; }

GLfloat *Transform::getMatrixPtr() { return &_modelMatrix[0][0]; }

glm::vec3 Transform::getPosition() const { return _position; }

glm::vec3 Transform::getRotation() const { return _rotation; }

glm::vec3 Transform::getScale() const { return _scale; }
