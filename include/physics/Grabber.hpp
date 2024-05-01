#pragma once

#include <glm/vec3.hpp>

struct Ray;

class Camera;
class Entity;
class SoftbodyObject;

class Grabber {
public:
  Grabber() = default;

  void setCamera(Camera *camera) { this->_camera = camera; }

  /**
   * @brief Attempts to grab a softbody object that intersects the ray
   *
   * @param ray The ray to intersect with
   * @param rootNode The root node of the scene graph
   */
  void grab(Ray ray, Entity &rootNode);

  /**
   * @brief Moves the grabbed object to the new grab point
   *
   * @param rayDir The direction of the new grab ray in view space
   */
  void moveGrabbed(const glm::vec3& rayDir) const;

  void release();

  void deleteObject(Ray ray, Entity &rootNode);

  bool isGrabbing() { return _grabbed != nullptr; };

private:
  Camera *_camera = nullptr;

  SoftbodyObject *_grabbed = nullptr;
  glm::vec3 _grabPoint;

  Entity *getHitEntity(Ray ray, Entity &rootNode);
};
