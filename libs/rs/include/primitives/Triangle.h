#pragma once
#include "geom/Mesh.h"

namespace rs {
namespace prim {

class Triangle : public geom::Mesh {
 public:
  Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2,
           bool b_wireframe = false) {
    vertices_ = {p0, p1, p2};
    auto v0 = p1 - p0;
    auto v1 = p2 - p0;
    auto n = glm::cross(v1, v0);
    n = glm::normalize(n);
    normals_ = {n, n, n};

    if (b_wireframe) {
      indices_ = {0, 1, 1, 2, 2, 0};
    } else {
      indices_ = {0, 1, 2};
    }
  }
};

}  // namespace prim
}  // namespace rs
