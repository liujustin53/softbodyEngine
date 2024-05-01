/** @file Transform.hpp
 *  @brief Responsible for holding matrix operations in model, view, and
 * projection space..
 *
 *  More...
 *
 *  @author Mike
 *  @bug No known bugs.
 */
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include <glad/glad.h>

// The purpose of this class is to store
// transformations of 3D entities (cameras, objects, etc.)
class Transform {
public:
  // Resets the transform
  void reset();
  // Computes the model matrix with a parent
  void computeModelMatrix(const glm::mat4 &parentModelMatrix = glm::mat4(1.0f));

  // Returns the transformation matrix
  glm::mat4 getModelMatrix() const;
  GLfloat *getMatrixPtr();

  glm::vec3 getPosition() const;
  glm::vec3 getRotation() const; // Returns the rotation in degrees
  glm::vec3 getScale() const;

  template <typename... Args> Transform &setPosition(Args &&...args) {
    _position = glm::vec3(std::forward<Args>(args)...);
    return *this;
  }

  template <typename... Args> Transform &setRotation(Args &&...args) {
    _rotation = glm::vec3(std::forward<Args>(args)...);
    return *this;
  }

  template <typename... Args> Transform &setScale(Args &&...args) {
    _scale = glm::vec3(std::forward<Args>(args)...);
    return *this;
  }

  template <typename... Args> Transform &translate(Args &&...args) {
    _position += glm::vec3(std::forward<Args>(args)...);
    return *this;
  }

  template <typename... Args> Transform &rotate(Args &&...args) {
    _rotation += glm::vec3(std::forward<Args>(args)...);
    return *this;
  }

  template <typename... Args> Transform &scale(Args &&...args) {
    _scale *= glm::vec3(std::forward<Args>(args)...);
    return *this;
  }

private:
  glm::mat4 getLocalModelMatrix() const;

  glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 _rotation = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f);

  // Stores the actual transformation matrix
  glm::mat4 _modelMatrix = glm::mat4(1.0f);
};

#endif
