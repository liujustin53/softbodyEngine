#include "physics/SoftbodyObject.hpp"

#include "core/AABB.hpp"
#include "core/ObjLoader.hpp"
#include "core/Ray.hpp"
#include "core/Transform.hpp"

glm::vec3 playSpace = glm::vec3(10.0f, 10.0f, 10.0f);

SoftbodyObject::SoftbodyObject(const SoftbodyMesh &softbodyMesh,
                               const glm::vec3 &color)
    : Object(color), _softbodyMesh(softbodyMesh) {
  _vertexBufferLayout.createSoftBodyBufferLayout(_softbodyMesh.pointMasses,
                                                 _softbodyMesh.faces);
}

SoftbodyObject::SoftbodyObject(const Mesh &mesh, const glm::vec3 &color)
    : Object(color), _softbodyMesh(mesh) {
  _vertexBufferLayout.createSoftBodyBufferLayout(_softbodyMesh.pointMasses,
                                                 _softbodyMesh.faces);
}

SoftbodyObject::SoftbodyObject(const std::string &filename) {
  Mesh mesh;
  std::vector<Texture> textures;

  ObjLoader::loadMesh(filename, mesh, textures);

  // Load textures
  addTextures(textures);

  _softbodyMesh = SoftbodyMesh(mesh);

  _vertexBufferLayout.createSoftBodyBufferLayout(_softbodyMesh.pointMasses,
                                                 _softbodyMesh.faces);
}

void SoftbodyObject::update(float deltaTime, Transform &transform) {
  // TODO: refactor into physics engine
  if (_isStatic) {
    return;
  }

  // Convert from local to world space
  const glm::mat4 modelMatrix = transform.getModelMatrix();
  for (auto &pointMass : _softbodyMesh.pointMasses) {
    pointMass.position = modelMatrix * glm::vec4(pointMass.position, 1.0f);
  }

  // Reset lambda values
  for (auto &edge : _softbodyMesh.edges) {
    edge.lambdaLength = 0.0f;
    edge.lambdaSpanLength = 0.0f;
    // edge.lambdaAngle = 0.0f;
  }
  _softbodyMesh.lambdaVolume = 0.0f;
  if (_grabbedFaceIdx != -1) {
    for (int i = 0; i < 3; i++) {
      _grabLengthLambdas[i] = 0.0f;
    }
  }

  // Run the simulation
  constexpr int iterations = 10;
  const float subTimeStep = deltaTime / iterations;
  for (int i = 0; i < iterations; i++) {
    preSolve(subTimeStep);
    handleCollision();
    solveConstraints(subTimeStep);
    if (_grabbedFaceIdx != -1) {
      moveGrabbed(subTimeStep);
    }
    postSolve(subTimeStep);
  }

  // Update the transform
  const glm::vec3 oldCenter = transform.getPosition();
  const glm::vec3 center = _softbodyMesh.getCenter();
  transform.setPosition(center);

  // Convert back from world to local space
  const glm::vec3 translation = center - oldCenter;
  const glm::mat4 inverseModelMatrix = glm::inverse(modelMatrix);
  for (auto &pointMass : _softbodyMesh.pointMasses) {
    pointMass.position -= translation;
    pointMass.position =
        inverseModelMatrix * glm::vec4(pointMass.position, 1.0f);
  }

  // Update the normals
  updateNormals();

  // Update the vertex buffer layout
  _vertexBufferLayout.updateSoftBodyBufferLayout(_softbodyMesh.pointMasses,
                                                 _softbodyMesh.faces);
}

void SoftbodyObject::applyForce(const glm::vec3 &force) {
  const glm::vec3 forcePerPoint =
      force / (float)_softbodyMesh.pointMasses.size();
  for (auto &pointMass : _softbodyMesh.pointMasses) {
    pointMass.velocity += forcePerPoint * pointMass.invMass;
  }
}

void SoftbodyObject::accelerate(const glm::vec3 &acceleration) {
  for (auto &pointMass : _softbodyMesh.pointMasses) {
    pointMass.velocity += acceleration;
  }
}

AABB SoftbodyObject::getAABB() const {
  glm::vec3 min = _softbodyMesh.pointMasses[0].position;
  glm::vec3 max = _softbodyMesh.pointMasses[0].position;
  for (const auto &pointMass : _softbodyMesh.pointMasses) {
    for (int i = 0; i < 3; i++) {
      min[i] = std::min(min[i], pointMass.position[i]);
      max[i] = std::max(max[i], pointMass.position[i]);
    }
  }

  return {min, max};
}

bool SoftbodyObject::grab(Ray &ray, const glm::mat4 &modelMatrix) {
  for (int i = 0; i < _softbodyMesh.faces.size(); i++) {
    const SoftbodyFace &face = _softbodyMesh.faces[i];
    glm::vec3 a = _softbodyMesh.pointMasses[face.pointMassIndices[0]].position;
    glm::vec3 b = _softbodyMesh.pointMasses[face.pointMassIndices[1]].position;
    glm::vec3 c = _softbodyMesh.pointMasses[face.pointMassIndices[2]].position;
    // Convert the face to world space
    a = modelMatrix * glm::vec4(a, 1.0f);
    b = modelMatrix * glm::vec4(b, 1.0f);
    c = modelMatrix * glm::vec4(c, 1.0f);

    const glm::vec3 ab = b - a;
    const glm::vec3 ac = c - a;
    const glm::vec3 normal = glm::cross(ab, ac);

    const float nDotAMinusRay = glm::dot(normal, a - ray.origin);
    const float nDotRay = glm::dot(normal, ray.dir);
    const float t = nDotAMinusRay / nDotRay;
    const glm::vec3 intersection = ray.origin + ray.dir * t;

    const glm::vec3 bc = c - b;
    const glm::vec3 ca = a - c;

    // Check if the intersection is inside the triangle
    // Check if the cross product of the edge and the intersection point is
    // pointing in the same direction as the normal
    const float dotCrossAB = glm::dot(glm::cross(ab, intersection - a), normal);
    const float dotCrossBC = glm::dot(glm::cross(bc, intersection - b), normal);
    const float dotCrossCA = glm::dot(glm::cross(ca, intersection - c), normal);

    if ((dotCrossAB >= 0.0f && dotCrossBC >= 0.0f && dotCrossCA >= 0.0f) ||
        (dotCrossAB <= 0.0f && dotCrossBC <= 0.0f && dotCrossCA <= 0.0f)) {
      ray.t = t;
      _grabbedFaceIdx = i;
      _grabPoint = intersection;
      // Calculate the rest distances
      _grabRestDistances[0] = glm::length(a - intersection);
      _grabRestDistances[1] = glm::length(b - intersection);
      _grabRestDistances[2] = glm::length(c - intersection);
      return true;
    }
  }

  ray.t = -1.0f;
  _grabbedFaceIdx = -1;
  return false;
}

void SoftbodyObject::moveGrabbed(float deltaTime) {
  // Simulate 3 distance constraints
  const float distanceAlpha =
      _softbodyMesh.distanceCompliance / std::pow(deltaTime, 2);

  const SoftbodyFace &face = _softbodyMesh.faces[_grabbedFaceIdx];
  PointMass &a = _softbodyMesh.pointMasses[face.pointMassIndices[0]];
  PointMass &b = _softbodyMesh.pointMasses[face.pointMassIndices[1]];
  PointMass &c = _softbodyMesh.pointMasses[face.pointMassIndices[2]];
  PointMass grabPoint;
  grabPoint.position = _grabPoint;
  grabPoint.invMass = 0.0f;

  solveDistanceConstraints(a, grabPoint, _grabRestDistances[0],
                           _grabLengthLambdas[0], distanceAlpha);
  solveDistanceConstraints(b, grabPoint, _grabRestDistances[1],
                           _grabLengthLambdas[1], distanceAlpha);
  solveDistanceConstraints(c, grabPoint, _grabRestDistances[2],
                           _grabLengthLambdas[2], distanceAlpha);
}

void SoftbodyObject::preSolve(float deltaTime) {
  // Integrate position and velocity
  for (auto &pointMass : _softbodyMesh.pointMasses) {
    // Update velocity
    const glm::vec3 nextVelocity =
        pointMass.velocity + glm::vec3(0.0f, -9.81f, 0.0f) * deltaTime;

    // Save the previous position
    pointMass.prevPosition = pointMass.position;

    // Update position
    pointMass.position += nextVelocity * deltaTime;
  }
}

void SoftbodyObject::solveConstraints(float deltaTime) {
  // Apply distance constraints
  const float distanceAlpha =
      _softbodyMesh.distanceCompliance / std::pow(deltaTime, 2);
  for (auto &edge : _softbodyMesh.edges) {
    PointMass &p0 = _softbodyMesh.pointMasses[edge.pointMassIndices[0]];
    PointMass &p1 = _softbodyMesh.pointMasses[edge.pointMassIndices[1]];
    solveDistanceConstraints(p0, p1, edge.restLength, edge.lambdaLength,
                             distanceAlpha);
  }

  // Apply volume constraint
  solveVolumeConstraint(deltaTime);

  // Apply bending constraints
  // solveBendingConstraints(deltaTime);

  // Angles don't work, create a distance constraint between each face instead
  const float bendingAlpha =
      _softbodyMesh.bendingCompliance / std::pow(deltaTime, 2);
  for (auto &edge : _softbodyMesh.edges) {
    PointMass &pL = _softbodyMesh.pointMasses[edge.neighborIndices[0]];
    PointMass &pR = _softbodyMesh.pointMasses[edge.neighborIndices[1]];
    solveDistanceConstraints(pL, pR, edge.restSpanLength, edge.lambdaSpanLength,
                             bendingAlpha);
  }
}

void SoftbodyObject::handleCollision() {
  for (auto &pointMass : _softbodyMesh.pointMasses) {
    // Collide with the ground
    if (pointMass.position.y < 0.0f) {
      pointMass.position = pointMass.prevPosition;
      pointMass.position.y = 0.0f;
    }

    // Collide with the play space
    if (std::fabs(pointMass.position.x) > playSpace.x) {
      pointMass.position = pointMass.prevPosition;
      pointMass.position.x = glm::sign(pointMass.position.x) * playSpace.x;
    }
    if (std::fabs(pointMass.position.y) > playSpace.y) {
      pointMass.position = pointMass.prevPosition;
      pointMass.position.y = glm::sign(pointMass.position.y) * playSpace.y;
    }
    if (std::fabs(pointMass.position.z) > playSpace.z) {
      pointMass.position = pointMass.prevPosition;
      pointMass.position.z = glm::sign(pointMass.position.z) * playSpace.z;
    }
  }
}

void SoftbodyObject::postSolve(float deltaTime) {
  const float oneOverDeltaTime = 1.0f / deltaTime;
  // Update the velocity
  for (auto &pointMass : _softbodyMesh.pointMasses) {
    pointMass.velocity =
        (pointMass.position - pointMass.prevPosition) * oneOverDeltaTime;
  }
}

void SoftbodyObject::solveDistanceConstraints(PointMass &p0, PointMass &p1,
                                              float restLength,
                                              float &lambdaLength,
                                              float alpha) {
  /*
   * C(p0, p1) = |p0 - p1| - InitialLength
   * dC(p0, p1)/dp0 = (p0 - p1) / |p0 - p1|
   * dC(p0, p1)/dp1 = -(p0 - p1) / |p0 - p1|
   * dL = (-C(p0, p1) - Alpha * lambda) / (m0 + m1 + Alpha)
   * deltaP = dL * m0 * dC(p0, p1)/dp0
   */
  const glm::vec3 delta = p0.position - p1.position;
  const float C = glm::length(delta) - restLength;
  if (std::fabs(C) < 0.0001f) {
    return;
  }

  const glm::vec3 dC = glm::normalize(delta);
  const float deltaLambda =
      (-C - alpha * lambdaLength) / (p0.invMass + p1.invMass + alpha);
  p0.position += deltaLambda * p0.invMass * dC;
  p1.position -= deltaLambda * p1.invMass * dC;
  lambdaLength += deltaLambda;
}

void SoftbodyObject::solveVolumeConstraint(float deltaTime) {
  // C = V - V0
  float C = _softbodyMesh.calculateVolume() -
            _softbodyMesh.pressure * _softbodyMesh.restVolume;
  // Limit constraint to prevent large changes
  const float maxC = _softbodyMesh.restVolume * 0.1f;
  C = std::max(std::min(C, maxC), -maxC);
  if (std::fabs(C) < 0.0001f) {
    return;
  }

  const float alpha = _softbodyMesh.volumeCompliance / std::pow(deltaTime, 2);

  // Calculate dC (gradient of the volume constraint function)
  std::vector<glm::vec3> dC(_softbodyMesh.pointMasses.size(), glm::vec3(0.0f));
  for (auto &face : _softbodyMesh.faces) {
    const unsigned int i0 = face.pointMassIndices[0];
    const unsigned int i1 = face.pointMassIndices[1];
    const unsigned int i2 = face.pointMassIndices[2];
    const glm::vec3 &p0 = _softbodyMesh.pointMasses[i0].position;
    const glm::vec3 &p1 = _softbodyMesh.pointMasses[i1].position;
    const glm::vec3 &p2 = _softbodyMesh.pointMasses[i2].position;

    dC[i0] += glm::cross(p1, p2) / 6.0f;
    dC[i1] += glm::cross(p2, p0) / 6.0f;
    dC[i2] += glm::cross(p0, p1) / 6.0f;
  }

  // Calculate dL denominator
  float denom = alpha;
  for (int i = 0; i < _softbodyMesh.pointMasses.size(); i++) {
    const PointMass &pointMass = _softbodyMesh.pointMasses[i];
    denom += pointMass.invMass * glm::dot(dC[i], dC[i]);
  }

  if (std::fabs(denom) < 0.0001f) {
    return;
  }

  // Calculate delta lambda
  const float deltaLambda = (-C - alpha * _softbodyMesh.lambdaVolume) / denom;
  for (int i = 0; i < _softbodyMesh.pointMasses.size(); i++) {
    PointMass &pointMass = _softbodyMesh.pointMasses[i];
    pointMass.position += deltaLambda * pointMass.invMass * dC[i];
  }
  _softbodyMesh.lambdaVolume += deltaLambda;
}

// void SoftbodyObject::solveBendingConstraints(float deltaTime) {
//   float alpha = _softbodyMesh.bendingCompliance / std::pow(deltaTime, 2);
//   for (auto &edge : _softbodyMesh.edges) {
//     // First get the 4 point masses
//     const SoftbodyFace &face0 = _softbodyMesh.faces[edge.faceIndices[0]];
//     const SoftbodyFace &face1 = _softbodyMesh.faces[edge.faceIndices[1]];
//     unsigned int i0 = edge.pointMassIndices[0];
//     unsigned int i1 = edge.pointMassIndices[1];
//     unsigned int i2 = 0;
//     for (unsigned int i = 0; i < 3; i++) {
//       unsigned int pointIdx = face0.pointMassIndices[i];
//       if (pointIdx != i0 && pointIdx != i1) {
//         i2 = pointIdx;
//         break;
//       }
//     }
//     unsigned int i3 = 0;
//     for (unsigned int i = 0; i < 3; i++) {
//       unsigned int pointIdx = face1.pointMassIndices[i];
//       if (pointIdx != i0 && pointIdx != i1) {
//         i3 = pointIdx;
//         break;
//       }
//     }
//     PointMass &p0 = _softbodyMesh.pointMasses[i0];
//     PointMass &p1 = _softbodyMesh.pointMasses[i1];
//     PointMass &p2 = _softbodyMesh.pointMasses[i2];
//     PointMass &p3 = _softbodyMesh.pointMasses[i3];
//
//     // The three edges
//     glm::vec3 eM = p1.position - p0.position;
//     glm::vec3 eL = p2.position - p0.position;
//     glm::vec3 eR = p3.position - p0.position;
//
//     // Calculate the normals
//     glm::vec3 nL = glm::cross(eM, eL);
//     glm::vec3 nR = glm::cross(eM, eR);
//     float nLLength = glm::length(nL);
//     float nRLength = glm::length(nR);
//     if (nLLength < 0.0001f || nRLength < 0.0001f) {
//       continue;
//     }
//     nL /= nLLength;
//     nR /= nRLength;
//
//     // Calculate the angle
//     float cosTheta = glm::dot(nL, nR);
//     // clamp cosTheta to prevent NaNs
//     cosTheta = std::max(-1.0f, std::min(1.0f, cosTheta));
//     float arcCosSign = 0.0f;
//     // float theta = calculateAngle(nL, nR, eM, arcCosSign);
//     float theta = std::acos(cosTheta);
//
//     // Constraint
//     float C = theta - edge.restAngle;
//     if (std::fabs(C) < 0.0001f) {
//       continue;
//     }
//
//     // lol math is for losers
//     glm::vec3 dP1 =
//         ((glm::cross(eR, nL) + cosTheta * glm::cross(nR, eR)) / nRLength +
//          (glm::cross(eL, nR) + cosTheta * glm::cross(nL, eL)) / nLLength);
//     glm::vec3 dP2 =
//         (glm::cross(nR, eM) - cosTheta * glm::cross(nL, eM)) / nLLength;
//     glm::vec3 dP3 =
//         (glm::cross(nL, eM) - cosTheta * glm::cross(nR, eM)) / nRLength;
//     glm::vec3 dP0 = -dP1 - dP2 - dP3;
//
//     float dPDenomRadicand = 1.0f - cosTheta * cosTheta;
//     float dPDenom = -arcCosSign * std::sqrt(dPDenomRadicand);
//
//     float lambdaDenom =
//         p0.invMass * glm::dot(dP0, dP0) + p1.invMass * glm::dot(dP1, dP1) +
//         p2.invMass * glm::dot(dP2, dP2) + p3.invMass * glm::dot(dP3, dP3) +
//         alpha * dPDenomRadicand;
//     if (std::fabs(lambdaDenom) < 0.0001f) {
//       continue;
//     }
//
//     float deltaLambdaRaw = (-C - alpha * edge.lambdaAngle) / lambdaDenom;
//     p0.position += (p0.invMass * dPDenom * deltaLambdaRaw) * dP0;
//     p1.position += (p1.invMass * dPDenom * deltaLambdaRaw) * dP1;
//     p2.position += (p2.invMass * dPDenom * deltaLambdaRaw) * dP2;
//     p3.position += (p3.invMass * dPDenom * deltaLambdaRaw) * dP3;
//     edge.lambdaAngle += dPDenomRadicand * deltaLambdaRaw;
//   }
// }

// float SoftbodyObject::calculateAngle(glm::vec3 nL, glm::vec3 nR, glm::vec3
// eM,
//                                      float &arcCosSign) {
//   float cosTheta = glm::dot(nL, nR);
//   float theta = 0.0f;
//
//   if (glm::dot(eM, glm::cross(nL, nR)) < 0.0f) {
//     theta = (2.0f * glm::pi<float>()) - std::acos(cosTheta);
//     arcCosSign = -1.0f;
//   } else {
//     theta = std::acos(cosTheta);
//     arcCosSign = 1.0f;
//   }
//
//   return theta;
// }

void SoftbodyObject::updateNormals() {
  for (const auto &face : _softbodyMesh.faces) {
    PointMass &a = _softbodyMesh.pointMasses[face.pointMassIndices[0]];
    PointMass &b = _softbodyMesh.pointMasses[face.pointMassIndices[1]];
    PointMass &c = _softbodyMesh.pointMasses[face.pointMassIndices[2]];

    const glm::vec3 normal =
        glm::cross(b.position - a.position, c.position - a.position);

    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  for (auto &pointMass : _softbodyMesh.pointMasses) {
    pointMass.normal = glm::normalize(pointMass.normal);
  }
}
