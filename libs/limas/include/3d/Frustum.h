#pragma once
#include "geom/Mesh.h"

namespace limas {

class Frustum {
 public:
  Frustum(const glm::mat4& mvp) {
    planes_.resize(6);

    // Left
    planes_[0] = glm::vec4(mvp[0][3] + mvp[0][0], mvp[1][3] + mvp[1][0],
                           mvp[2][3] + mvp[2][0], mvp[3][3] + mvp[3][0]);

    // Right
    planes_[1] = glm::vec4(mvp[0][3] - mvp[0][0], mvp[1][3] - mvp[1][0],
                           mvp[2][3] - mvp[2][0], mvp[3][3] - mvp[3][0]);

    // Top
    planes_[2] = glm::vec4(mvp[0][3] - mvp[0][1], mvp[1][3] - mvp[1][1],
                           mvp[2][3] - mvp[2][1], mvp[3][3] - mvp[3][1]);

    // Bottom
    planes_[3] = glm::vec4(mvp[0][3] + mvp[0][1], mvp[1][3] + mvp[1][1],
                           mvp[2][3] + mvp[2][1], mvp[3][3] + mvp[3][1]);

    // Near
    planes_[4] = glm::vec4(mvp[0][3] + mvp[0][2], mvp[1][3] + mvp[1][2],
                           mvp[2][3] + mvp[2][2], mvp[3][3] + mvp[3][2]);

    // Far
    planes_[5] = glm::vec4(mvp[0][3] - mvp[0][2], mvp[1][3] - mvp[1][2],
                           mvp[2][3] - mvp[2][2], mvp[3][3] - mvp[3][2]);

    for (int i = 0; i < 6; i++) {
      float length = glm::length(glm::vec3(planes_[i]));
      planes_[i] /= length;
    }

    static glm::vec4 clip_space_vertices[8] = {
        {-1, -1, -1, 1}, {1, -1, -1, 1}, {1, 1, -1, 1}, {-1, 1, -1, 1},
        {-1, -1, 1, 1},  {1, -1, 1, 1},  {1, 1, 1, 1},  {-1, 1, 1, 1}};

    vertices_.resize(8);
    glm::mat4 inv_mvp = glm::inverse(mvp);
    for (int i = 0; i < 8; i++) {
      glm::vec4 v = inv_mvp * clip_space_vertices[i];
      v /= v.w;
      ;
      vertices_[i] = glm::vec3(v);
    }
  }

  bool isPointInside(const glm::vec3& point) const {
    for (int i = 0; i < 6; i++) {
      if (glm::dot(glm::vec3(planes_[i]), point) + planes_[i].w < 0.0f) {
        return false;
      }
    }
    return true;
  }

  bool isBoundingBoxInside(const glm::vec3& min, const glm::vec3& max) const {
    glm::vec3 vertices[8] = {{min.x, min.y, min.z}, {max.x, min.y, min.z},
                             {max.x, max.y, min.z}, {min.x, max.y, min.z},
                             {min.x, max.y, max.z}, {max.x, max.y, max.z},
                             {max.x, min.y, max.z}, {min.x, min.y, max.z}};

    for (int i = 0; i < 6; i++) {
      int num_outside = 0;
      for (int j = 0; j < 8; j++) {
        if (glm::dot(glm::vec3(planes_[i]), vertices[j]) + planes_[i].w <
            0.0f) {
          num_outside++;
        }
      }
      if (num_outside == 8) {
        return false;
      }
    }
    return true;
  }

  bool isBoundingSphereInside(const glm::vec3& center, float radius) const {
    for (int i = 0; i < 6; i++) {
      float distance = glm::dot(glm::vec3(planes_[i]), center) + planes_[i].w;
      if (distance < -radius) {
        return false;
      }
    }
    return true;
  }

  geom::Mesh toMesh() const {
    static const std::vector<int> indices({
        0, 1, 1, 2, 2, 3, 3, 0,  // Near plane
        4, 5, 5, 6, 6, 7, 7, 4,  // Far plane
        0, 4, 1, 5, 2, 6, 3, 7   // Connecting lines
    });

    geom::Mesh mesh;
    mesh.setVertices(vertices_);
    mesh.setIndices(indices);
    mesh.fillColor(glm::vec4(1));

    return mesh;
  }

 protected:
  std::vector<glm::vec4> planes_;
  std::vector<glm::vec3> vertices_;
};

}  // namespace limas
