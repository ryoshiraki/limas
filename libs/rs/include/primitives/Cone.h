#pragma once
#include "geom/Mesh.h"

namespace rs {
namespace prim {

class Cone : public geom::Mesh {
 public:
  Cone(float radius, int height, int num_segments_radius,
       int num_segments_height, bool b_capped = true, bool b_wireframe = false)
      : radius_(radius), height_(height) {
    // Compute positions and indices for the sides
    for (int y = 0; y <= num_segments_height; ++y) {
      for (int x = 0; x <= num_segments_radius; ++x) {
        float theta = x * 2.0f * M_PI / num_segments_radius;
        float h_ratio = y / (float)num_segments_height;

        glm::vec3 v =
            glm::vec3((1 - h_ratio) * radius * cos(theta), h_ratio * height,
                      (1 - h_ratio) * radius * sin(theta));
        glm::vec3 side_center = glm::vec3(0.0f, h_ratio * height, 0.0f);

        glm::vec3 n = glm::normalize(v - side_center);
        glm::vec2 t = glm::vec2((float)x / num_segments_radius, h_ratio);

        vertices_.push_back(v);
        normals_.push_back(n);
        texcoords_.push_back(t);
      }
    }

    for (int y = 0; y < num_segments_height; ++y) {
      for (int x = 0; x < num_segments_radius; ++x) {
        int start = y * (num_segments_radius + 1) + x;
        indices_.push_back(start);
        indices_.push_back(start + 1);
        indices_.push_back(start + num_segments_radius + 2);

        indices_.push_back(start);
        indices_.push_back(start + num_segments_radius + 2);
        indices_.push_back(start + num_segments_radius + 1);
      }
    }

    // Compute positions and indices for the bottom cap
    if (b_capped) {
      // Bottom cap
      for (int i = 0; i <= num_segments_radius; ++i) {
        float theta = i * 2.0f * M_PI / num_segments_radius;

        glm::vec3 v = glm::vec3(radius * cos(theta), 0.0f, radius * sin(theta));
        glm::vec3 n = glm::vec3(0.0f, -1.0f, 0.0f);
        glm::vec2 t = glm::vec2(v.x / (2.0f * radius) + 0.5f,
                                v.z / (2.0f * radius) + 0.5f);

        vertices_.push_back(v);
        normals_.push_back(n);
        texcoords_.push_back(t);
      }

      int start_index = (num_segments_height + 1) * (num_segments_radius + 1);
      for (int i = 1; i <= num_segments_radius - 1; ++i) {
        indices_.push_back(start_index);
        indices_.push_back(start_index + i);
        indices_.push_back(start_index + i + 1);
      }
    }
  }

 private:
  float radius_, height_;
};

}  // namespace prim
}  // namespace rs
