#pragma once
#include "geom/Mesh.h"
#include "math/Math.h"

namespace limas {
namespace prim {

class Sphere : public geom::Mesh {
 public:
  Sphere(float radius, int res_x, int res_y, bool b_wireframe = false)
      : radius_(radius) {
    if (res_x <= 0 || res_y <= 0) {
      throw std::invalid_argument("res_x and res_y must be positive integers.");
    }

    for (int y = 0; y <= res_y; ++y) {
      for (int x = 0; x <= res_x; ++x) {
        float s = static_cast<float>(x) / res_x;
        float t = static_cast<float>(y) / res_y;

        glm::vec3 pos{
            radius_ * cos(math::pi() * s * 2.0f) * sin(math::pi() * t),
            radius_ * cos(math::pi() * t),
            radius_ * sin(math::pi() * s * 2.0f) * sin(math::pi() * t)};

        vertices_.push_back(pos);
        normals_.push_back(glm::normalize(pos));
        texcoords_.push_back({s, t});
      }
    }

    if (b_wireframe) {
      for (int y = 0; y < res_y; ++y) {
        for (int x = 0; x < res_x; ++x) {
          int i0 = x + y * (res_x + 1);
          int i1 = (x + 1) + y * (res_x + 1);
          int i2 = x + (y + 1) * (res_x + 1);
          int i3 = (x + 1) + (y + 1) * (res_x + 1);

          indices_.push_back(i0);
          indices_.push_back(i1);

          indices_.push_back(i1);
          indices_.push_back(i3);

          indices_.push_back(i3);
          indices_.push_back(i2);

          indices_.push_back(i2);
          indices_.push_back(i0);
        }
      }
    } else {
      for (int y = 0; y < res_y; ++y) {
        for (int x = 0; x < res_x; ++x) {
          int i0 = x + y * (res_x + 1);
          int i1 = (x + 1) + y * (res_x + 1);
          int i2 = x + (y + 1) * (res_x + 1);
          int i3 = (x + 1) + (y + 1) * (res_x + 1);

          indices_.push_back(i0);
          indices_.push_back(i1);
          indices_.push_back(i3);

          indices_.push_back(i3);
          indices_.push_back(i2);
          indices_.push_back(i0);
        }
      }
    }
  }

 private:
  float radius_;
};

}  // namespace prim
}  // namespace limas
