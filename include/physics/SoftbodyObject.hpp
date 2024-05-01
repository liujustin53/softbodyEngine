#pragma once

#include "core/Object.hpp"

#include "physics/SoftbodyMesh.hpp"

#include <string>

struct Ray;

struct AABB;

class SoftbodyObject : public Object {
public:
  SoftbodyObject(const SoftbodyMesh &softbodyMesh,
                 const glm::vec3 &color = glm::vec3(1.0f));
  SoftbodyObject(const Mesh &mesh, const glm::vec3 &color = glm::vec3(1.0f));
  SoftbodyObject(const std::string &filename);

  virtual void update(float deltaTime, Transform &transform) override;

  bool isStatic() const { return _isStatic; }
  void setStatic(bool isStatic) { _isStatic = isStatic; }

  void applyForce(const glm::vec3 &force);
  void accelerate(const glm::vec3 &acceleration);

  /**
   * @brief Calculates the axis-aligned bounding box of the softbody object
   *
   * @return AABB The axis-aligned bounding box
   */
  AABB getAABB() const;

  /**
   * @brief Attempts to grab a face on the softbody that intersects the ray
   * and modifies the ray t value.
   *
   * @param ray The ray to intersect with
   * @param modelMatrix The model matrix of the object
   * @return bool True if a face was grabbed
   */
  bool grab(Ray &ray, const glm::mat4 &modelMatrix);

  void updateGrabPoint(const glm::vec3 &grabPoint) { _grabPoint = grabPoint; };

  /**
   * @brief Releases the grabbed face
   */
  void release() { _grabbedFaceIdx = -1; };

protected:
  virtual unsigned int indicesCount() const override {
    return _softbodyMesh.faces.size() * 3;
  }

private:
  SoftbodyMesh _softbodyMesh;

  bool _isStatic = false;

  // Grabbing information
  int _grabbedFaceIdx = -1;
  glm::vec3 _grabPoint;        // The point where the face was grabbed.
  float _grabRestDistances[3]; // Rest distances from the initial grabbed
                               // point to the vertices of the grabbed face.
  float _grabLengthLambdas[3]; // Lambda values for the distance constraints.

  void moveGrabbed(float deltaTime);

  // Simulation helpers
  void preSolve(float deltaTime);
  void solveConstraints(float deltaTime);
  void handleCollision();
  void postSolve(float deltaTime);

  // Physics helpers
  void solveDistanceConstraints(PointMass &p0, PointMass &p1, float restLength,
                                float &lambdaLength, float alpha);
  void solveVolumeConstraint(float deltaTime);
  // void solveBendingConstraints(float deltaTime);

  // Calculates the angle between two normals accounting for the signs
  // float calculateAngle(glm::vec3 nL, glm::vec3 nR, glm::vec3 eM,
  //                      float &arcCosSign);

  // Rendering information update helpers
  void updateNormals();
};
