#include "core/Entity.hpp"

#include "core/Object.hpp"

#include "rendering/Shader.hpp"

#include <algorithm>

void Entity::update(float deltaTime) {
  if (_parent) {
    _transform.computeModelMatrix(_parent->_transform.getModelMatrix());
  } else {
    _transform.computeModelMatrix();
  }

  if (_object) {
    _object->update(deltaTime, _transform);
  }

  for (auto &child : _children) {
    child->update(deltaTime);
  }
}

void Entity::draw(const Shader &shader) const {
  if (_object) {
    shader.setMat4("u_Model", _transform.getModelMatrix());
    _object->draw(shader);
  }

  for (auto &child : _children) {
    child->draw(shader);
  }
}

void Entity::removeChild(Entity *child) {
  auto it = std::find_if(_children.begin(), _children.end(),
                         [child](const std::unique_ptr<Entity> &entity) {
                           return entity.get() == child;
                         });

  if (it != _children.end()) {
    _children.erase(it);
  }
}

void Entity::updateAABB() {
  if (_object) {
    _aabb = _object->getAABB();
  }
}

bool Entity::intersects(const Entity &other) const {
  return _aabb.intersects(other._aabb, other._transform.getModelMatrix(),
                          _transform.getModelMatrix());
}

float Entity::intersects(const Ray &ray) const {
  return _aabb.intersects(ray, _transform.getModelMatrix());
}

void Entity::traverse(std::vector<Entity *> &entities) {
  entities.push_back(this);

  for (auto &child : _children) {
    child->traverse(entities);
  }
}
