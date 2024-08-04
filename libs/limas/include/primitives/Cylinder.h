#pragma once
#include "geom/Mesh.h"

namespace rs {
namespace prim {

class Cylinder : public geom::Mesh {
 public:
  Cylinder(float radius, int height, int num_segments_radius,
           int num_segments_height, bool b_capped = true)
      : radius_(radius), height_(height) {
    for (int y = 0; y <= num_segments_height; ++y) {
      for (int x = 0; x <= num_segments_radius; ++x) {
        float theta = x * 2.0f * M_PI / num_segments_radius;

        glm::vec3 v = glm::vec3(radius * cos(theta),
                                height * y / (float)num_segments_height,
                                radius * sin(theta));
        glm::vec3 n = glm::normalize(glm::vec3(v.x, 0.0f, v.z));
        glm::vec2 t = glm::vec2((float)x / num_segments_radius,
                                (float)y / num_segments_height);

        vertices_.push_back(v);
        normals_.push_back(n);
        texcoords_.push_back(t);
      }
    }

    // Calculate indices here
    // Again, this is just a concept, you need to calculate indices according to
    // the cylinder you want to build
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

    if (b_capped) {
      // Compute positions and indices for the bottom cap
      for (int i = 0; i <= num_segments_radius; ++i) {
        float theta = i * 2.0f * M_PI / num_segments_radius;

        glm::vec3 v = glm::vec3(radius * cos(theta), 0.0f, radius * sin(theta));
        glm::vec3 n = glm::vec3(
            0.0f, -1.0f, 0.0f);  // Normals point downwards for the bottom cap
        glm::vec2 t = glm::vec2(
            v.x / (2.0f * radius) + 0.5f,
            v.z / (2.0f * radius) +
                0.5f);  // Texture coordinates are distributed radially

        vertices_.push_back(v);
        normals_.push_back(n);
        texcoords_.push_back(t);
      }

      // Compute indices for the bottom cap
      int start_index = (num_segments_height + 1) * (num_segments_radius + 1);
      for (int i = 1; i <= num_segments_radius - 1; ++i) {
        indices_.push_back(start_index + 0);
        indices_.push_back(start_index + i);
        indices_.push_back(start_index + i + 1);
      }

      // Compute positions and indices for the top cap
      for (int i = 0; i <= num_segments_radius; ++i) {
        float theta = i * 2.0f * M_PI / num_segments_radius;

        glm::vec3 v =
            glm::vec3(radius * cos(theta), height, radius * sin(theta));
        glm::vec3 n = glm::vec3(0.0f, 1.0f,
                                0.0f);  // Normals point upwards for the top cap
        glm::vec2 t = glm::vec2(
            v.x / (2.0f * radius) + 0.5f,
            v.z / (2.0f * radius) +
                0.5f);  // Texture coordinates are distributed radially

        vertices_.push_back(v);
        normals_.push_back(n);
        texcoords_.push_back(t);
      }

      // Compute indices for the top cap
      start_index += num_segments_radius + 1;
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