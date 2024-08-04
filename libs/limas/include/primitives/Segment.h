#pragma once
#include "geom/Mesh.h"

namespace limas {
namespace prim {

class Segment : public geom::Mesh {
 public:
  Segment(const glm::vec3& p0, const glm::vec3& p1) : p0_(p0), p1_(p1) {
    vertices_ = {p0, p1};
    texcoords_ = {{0, 0}, {1, 1}};
  }

  Segment(float x0, float y0, float z0, float x1, float y1, float z1)
      : Segment(glm::vec3(x0, y0, z0), glm::vec3(x1, y1, z1)) {}

  glm::vec3 getDirecton() const { return glm::normalize(p1_ - p0_); }
  float getLength() const { return glm::length(p1_ - p0_); }

 private:
  Segment() {}
  glm::vec3 p0_, p1_;
};

}  // namespace prim
}  // namespace limas
