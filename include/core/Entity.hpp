#pragma once

#include <memory>
#include <vector>

#include "AABB.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Transform.hpp"

#include "physics/SoftbodyObject.hpp"

class Shader;

class Entity {
public:
  Entity() = default;

  virtual void update(float deltaTime);
  virtual void draw(const Shader &shader) const;

  /**
   * @brief Add a child to the entity.
   *
   * @tparam Args
   * @param args Arguments to pass to the object constructor
   * @return The newly added child entity
   */
  template <typename... Args> Entity *addChild(Args &&...args) {
    Entity *child = _children.emplace_back(std::make_unique<Entity>()).get();
    child->_parent = this;
    child->setObject(std::forward<Args>(args)...);
    return child;
  }

  void removeChild(Entity *child);

  Entity *getParent() { return _parent; }

  /**
   * @brief Set the object of the entity.
   *
   * @tparam Args
   * @param args Arguments to pass to the object constructor
   */
  template <typename... Args> void setObject(Args &&...args) {
    _object = std::make_unique<SoftbodyObject>(std::forward<Args>(args)...);
    updateAABB();
  }

  SoftbodyObject *getObject() { return _object.get(); }

  Transform &getTransform() { return _transform; }
  const Transform &getTransform() const { return _transform; }
  void setTransform(const Transform &transform) { _transform = transform; }

  /**
   * @brief Update the AABB of the entity.
   * This should only be called once after the object has been set.
   * Too expensive to call every frame.
   */
  void updateAABB();

  /**
   * @brief Check if the entity intersects with another entity.
   *
   * @param other Entity to check intersection with
   * @return bool True if the entities intersect
   */
  bool intersects(const Entity &other) const;

  /**
   * @brief Check if the entity intersects with a ray.
   *
   * @param ray Ray to check intersection with
   * @return float The t distance to intersection point, negative if no
   * intersection
   */
  float intersects(const Ray &ray) const;

  /**
   * @brief Traverse the scene graph and add all entities to the vector.
   *
   * @param entities Vector to add entities to
   */
  void traverse(std::vector<Entity *> &entities);

protected:
  Transform _transform;

private:
  std::unique_ptr<SoftbodyObject> _object;
  AABB _aabb;

  // Scene graph
  std::vector<std::unique_ptr<Entity>> _children;
  Entity *_parent = nullptr;
};
