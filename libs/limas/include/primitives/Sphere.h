#pragma once
#include "geom/Mesh.h"

namespace rs {
namespace prim {

class Sphere : public geom::Mesh {
 public:
  Sphere(float radius, int res_x, int res_y, bool b_wireframe = false)
      : radius_(radius) {
    for (int y = 0; y <= res_y; ++y) {
      for (int x = 0; x <= res_x; ++x) {
        float x_seg = static_cast<float>(x) / static_cast<float>(res_x);
        float y_seg = static_cast<float>(y) / static_cast<float>(res_y);

        glm::vec3 pos{radius * cos(glm::pi<float>() * x_seg * 2.0f) *
                          sin(glm::pi<float>() * y_seg),
                      radius * cos(glm::pi<float>() * y_seg),
                      radius * sin(glm::pi<float>() * x_seg * 2.0f) *
                          sin(glm::pi<float>() * y_seg)};

        vertices_.push_back(pos);
        normals_.push_back(glm::normalize(pos));
        texcoords_.push_back({x_seg, y_seg});
      }
    }

    if (b_wireframe) {
      for (int y = 0; y < res_y; ++y) {
        for (int x = 0; x < res_x; ++x) {
          int bl = y * (res_x + 1) + x;
          int br = y * (res_x + 1) + x + 1;
          int tl = (y + 1) * (res_x + 1) + x;
          int tr = (y + 1) * (res_x + 1) + x + 1;

          indices_.push_back(bl);
          indices_.push_back(br);
          indices_.push_back(bl);
          indices_.push_back(tl);

          if (y != res_y - 1) {
            indices_.push_back(tl);
            indices_.push_back(tr);
          }

          if (x != res_x - 1) {
            indices_.push_back(br);
            indices_.push_back(tr);
          }
        }
      }
    } else {
      for (int y = 0; y < res_y; ++y) {
        for (int x = 0; x < res_x; ++x) {
          indices_.push_back(y * (res_x + 1) + x);
          indices_.push_back((y + 1) * (res_x + 1) + x);
          indices_.push_back((y + 1) * (res_x + 1) + x + 1);

          indices_.push_back(y * (res_x + 1) + x);
          indices_.push_back((y + 1) * (res_x + 1) + x + 1);
          indices_.push_back(y * (res_x + 1) + x + 1);
        }
      }
    }
  }

 private:
  float radius_;
};

}  // namespace prim
}  // namespace rs
