#include "physics/SoftbodyMesh.hpp"

SoftbodyMesh::SoftbodyMesh(const Mesh &mesh) {
  // Create point masses
  for (const auto &vertex : mesh._vertices) {
    PointMass pointMass;
    pointMass.position = vertex.position;
    pointMass.uv = vertex.uv;
    pointMass.normal = vertex.normal;
    pointMass.velocity = glm::vec3(0.0f);

    pointMasses.push_back(pointMass);
  }

  // Create faces
  for (unsigned int i = 0; i < mesh._indices.size(); i += 3) {
    SoftbodyFace face;
    face.pointMassIndices[0] = mesh._indices[i];
    face.pointMassIndices[1] = mesh._indices[i + 1];
    face.pointMassIndices[2] = mesh._indices[i + 2];

    faces.push_back(face);
  }

  // Create edges
  for (unsigned int i = 0; i < faces.size(); i++) {
    for (unsigned int j = 0; j < 3; j++) {
      unsigned int a = faces[i].pointMassIndices[j];
      unsigned int b = faces[i].pointMassIndices[(j + 1) % 3];

      // Check if the edge already exists
      bool edgeExists = false;
      for (auto &edge : edges) {
        if ((edge.pointMassIndices[0] == a && edge.pointMassIndices[1] == b) ||
            (edge.pointMassIndices[0] == b && edge.pointMassIndices[1] == a)) {
          // The edge already exists, add the current face to the edge
          edge.faceIndices[1] = i;
          edgeExists = true;
          break;
        }
      }
      if (edgeExists) {
        continue;
      }

      // Create a new edge
      SoftbodyEdge edge;
      edge.pointMassIndices[0] = a;
      edge.pointMassIndices[1] = b;

      edge.faceIndices[0] = i;

      edge.restLength =
          glm::length(pointMasses[a].position - pointMasses[b].position);

      edges.push_back(edge);
    }
  }

  // Calculate the rest angles of the edges
  // for (auto &edge : edges) {
  //   const SoftbodyFace &face0 = faces[edge.faceIndices[0]];
  //   const SoftbodyFace &face1 = faces[edge.faceIndices[1]];
  //   unsigned int i0 = edge.pointMassIndices[0];
  //   unsigned int i1 = edge.pointMassIndices[1];
  //   unsigned int i2 = 0;
  //   for (unsigned int i = 0; i < 3; i++) {
  //     unsigned int pointIdx = face0.pointMassIndices[i];
  //     if (pointIdx != i0 && pointIdx != i1) {
  //       i2 = pointIdx;
  //       break;
  //     }
  //   }
  //   unsigned int i3 = 0;
  //   for (unsigned int i = 0; i < 3; i++) {
  //     unsigned int pointIdx = face1.pointMassIndices[i];
  //     if (pointIdx != i0 && pointIdx != i1) {
  //       i3 = pointIdx;
  //       break;
  //     }
  //   }
  //   const PointMass &p0 = pointMasses[i0];
  //   const PointMass &p1 = pointMasses[i1];
  //   const PointMass &p2 = pointMasses[i2];
  //   const PointMass &p3 = pointMasses[i3];
  //
  //   // The three edges
  //   glm::vec3 eM = p1.position - p0.position;
  //   glm::vec3 eL = p2.position - p0.position;
  //   glm::vec3 eR = p3.position - p0.position;
  //
  //   // Calculate the normals
  //   glm::vec3 n0 = glm::normalize(glm::cross(eM, eL));
  //   glm::vec3 n1 = glm::normalize(glm::cross(eM, eR));
  //
  //   // Calculate the rest angle
  //   edge.restAngle = std::acos(glm::dot(n0, n1));
  // }

  // Calculate the rest span length of the faces
  for (auto &edge : edges) {
    unsigned int i0 = edge.pointMassIndices[0];
    unsigned int i1 = edge.pointMassIndices[1];
    unsigned int iL = 0;
    for (unsigned int i = 0; i < 3; i++) {
      unsigned int pointIdx = faces[edge.faceIndices[0]].pointMassIndices[i];
      if (pointIdx != i0 && pointIdx != i1) {
        iL = pointIdx;
        break;
      }
    }
    unsigned int iR = 0;
    for (unsigned int i = 0; i < 3; i++) {
      unsigned int pointIdx = faces[edge.faceIndices[1]].pointMassIndices[i];
      if (pointIdx != i0 && pointIdx != i1) {
        iR = pointIdx;
        break;
      }
    }

    // The two points of the faces that are not part of the edge
    edge.neighborIndices[0] = iL;
    edge.neighborIndices[1] = iR;
    const PointMass &pL = pointMasses[iL];
    const PointMass &pR = pointMasses[iR];

    // Calculate the rest span length
    edge.restSpanLength = glm::length(pL.position - pR.position);
  }

  // Calculate the volume of the mesh
  restVolume = 0.0f;
  for (const auto &face : faces) {
    glm::vec3 a = pointMasses[face.pointMassIndices[0]].position;
    glm::vec3 b = pointMasses[face.pointMassIndices[1]].position;
    glm::vec3 c = pointMasses[face.pointMassIndices[2]].position;

    float volume = glm::dot(a, glm::cross(b, c)) / 6.0f;
    restVolume += volume;

    // Calculate the inverse mass
    float mass = volume / 3.0f;
    for (unsigned int i = 0; i < 3; i++) {
      pointMasses[face.pointMassIndices[i]].invMass += mass;
    }
  }
  restVolume = std::fabs(restVolume);

  // Cap the inverse mass at 4 times the target to prevent instability
  float targetInvMass = pointMasses.size() / restVolume;
  for (auto &pointMass : pointMasses) {
    pointMass.invMass = 1.0f / pointMass.invMass;
    pointMass.invMass =
        std::min(std::fabs(pointMass.invMass), targetInvMass * 4.0f);
  }
}

float SoftbodyMesh::calculateVolume() const {
  float currentVolume = 0.0f;
  for (const auto &face : faces) {
    glm::vec3 a = pointMasses[face.pointMassIndices[0]].position;
    glm::vec3 b = pointMasses[face.pointMassIndices[1]].position;
    glm::vec3 c = pointMasses[face.pointMassIndices[2]].position;

    currentVolume += glm::dot(a, glm::cross(b, c)) / 6.0f;
  }

  return std::fabs(currentVolume);
}

glm::vec3 SoftbodyMesh::getCenter() const {
  glm::vec3 center(0.0f);
  for (const auto &pointMass : pointMasses) {
    center += pointMass.position;
  }

  return center / static_cast<float>(pointMasses.size());
}
