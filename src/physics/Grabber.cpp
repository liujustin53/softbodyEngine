#include "physics/Grabber.hpp"

#include "core/Camera.hpp"
#include "core/Entity.hpp"
#include "core/Ray.hpp"

#include "physics/SoftbodyObject.hpp"

void Grabber::grab(Ray ray, Entity &rootNode) {
  if (_grabbed) {
    return;
  }

  Entity *hit = getHitEntity(ray, rootNode);

  // Check for no hit, or if the hit entity does not have an object, or if the
  // object is static
  SoftbodyObject *object;
  if (!hit || !(object = hit->getObject()) || object->isStatic()) {
    return;
  }

  // Try to grab a face of the object
  if (!object->grab(ray, hit->getTransform().getModelMatrix())) {
    return;
  }

  _grabbed = object;
  // Calculate the point where the face was grabbed
  _grabPoint = ray.origin + ray.dir * ray.t;
  // Convert the point to view space
  _grabPoint = _camera->getViewMatrix() * glm::vec4(_grabPoint, 1.0f);
}

void Grabber::moveGrabbed(const glm::vec3 &rayDir) const {
  if (!_grabbed) {
    return;
  }

  // Calculate the t distance to the grabbed point
  float t = _grabPoint.z / rayDir.z;

  glm::vec3 point = rayDir * t;
  // Convert the point to world space
  point = glm::inverse(_camera->getViewMatrix()) * glm::vec4(point, 1.0f);

  _grabbed->updateGrabPoint(point);
}

void Grabber::release() {
  if (!_grabbed) {
    return;
  }

  _grabbed->release();
  _grabbed = nullptr;
}

void Grabber::deleteObject(Ray ray, Entity &rootNode) {
  Entity *hit = getHitEntity(ray, rootNode);

  if (!hit) {
    return;
  }

  // Check for no hit, or if the hit entity does not have an object, or if the
  // object is static
  SoftbodyObject *object;
  if (!hit || !(object = hit->getObject()) || object->isStatic()) {
    return;
  }

  // Check if the object is the grabbed object
  if (_grabbed == object) {
    release();
  }

  // Remove the object from the scene graph
  hit->getParent()->removeChild(hit);
}

Entity *Grabber::getHitEntity(Ray ray, Entity &rootNode) {
  // Traverse the scene graph and check for intersections
  std::vector<Entity *> entities;
  rootNode.traverse(entities);

  Entity *hit = nullptr;
  float t = std::numeric_limits<float>::max();
  for (auto entity : entities) {
    float tEntity = entity->intersects(ray);
    // If the intersection is closer than the previous closest intersection
    if (tEntity < t && tEntity >= 0.0f) {
      t = tEntity;
      hit = entity;
    }
  }

  return hit;
}
