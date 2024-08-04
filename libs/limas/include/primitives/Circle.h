#pragma once
#include "geom/Mesh.h"

namespace rs {
namespace prim {

class Ellipse : public geom::Mesh {
 public:
  Ellipse(const glm::vec2& center, float ra, float rb, int res)
      : center_(center), ra_(ra), rb_(rb) {
    for (int i = 0; i < res; i++) {
      float rad = ((float)i / (float)res) * (2.0 * glm::pi<float>()) -
                  glm::pi<float>() * 0.5;
      float x = center_.x + ra * cos(rad);
      float y = center_.y + rb * sin(rad);
      vertices_.emplace_back(glm::vec3(x, y, center_.x));
    }
  }

  const glm::vec2& getCenter() const { return center_; }
  float getRadiusA() const { return ra_; }
  float getRadiusB() const { return rb_; }

 protected:
  Ellipse() {}
  glm::vec2 center_;
  float ra_;
  float rb_;
};

class Circle : public Ellipse {
 public:
  Circle(const glm::vec2& center, float r, int res)
      : Ellipse(center, r, r, res) {}

  float getRadius() const { return ra_; }
};

}  // namespace prim
}  // namespace rs
