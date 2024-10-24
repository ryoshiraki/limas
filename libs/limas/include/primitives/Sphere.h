#pragma once
#include "geom/Mesh.h"
#include "math/Math.h"

namespace limas {
namespace prim {

class Sphere : public geom::Mesh {
 public:
  Sphere(float radius, int res_x, int res_y, bool b_wireframe = false)
      : radius_(radius) {
    for (int y = 0; y <= res_y; ++y) {
      for (int x = 0; x <= res_x; ++x) {
        float s = static_cast<float>(x) / static_cast<float>(res_x);
        float t = static_cast<float>(y) / static_cast<float>(res_y);

        glm::vec3 pos{
            radius * cos(math::pi() * s * 2.0f) * sin(math::pi() * t),
            radius * cos(math::pi() * t),
            radius * sin(math::pi() * s * 2.0f) * sin(math::pi() * t)};

        vertices_.push_back(pos);
        normals_.push_back(glm::normalize(pos));
        texcoords_.push_back({s, t});
      }
    }

    if (b_wireframe) {
      for (int y = 0; y < res_y; ++y) {
        for (int x = 0; x < res_x; ++x) {
          int i0 = res_x + res_y * res_x;
          int i1 = res_x + 1 + res_y * res_x;

          int i2 = res_x + (res_y + 1) * res_x;
          int i3 = res_x + 1 + (res_y + 1) * res_x;

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
          int i0 = res_x + res_y * res_x;
          int i1 = res_x + 1 + res_y * res_x;

          int i2 = res_x + (res_y + 1) * res_x;
          int i3 = res_x + 1 + (res_y + 1) * res_x;

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
