#pragma once
#include "geom/Mesh.h"

namespace rs {
namespace prim {

class Dot : public geom::Mesh {
 public:
  Dot(const glm::vec3& p) { vertices_ = {p}; }
  Dot(float x, float y, float z) : Dot(glm::vec3(x, y, z)) {}

  glm::vec3& operator=(const Dot& dot) { return vertices_[0]; }
  void operator=(const glm::vec3& p) { vertices_[0] = p; }

 protected:
  Dot() {}
};

}  // namespace prim
}  // namespace rs
