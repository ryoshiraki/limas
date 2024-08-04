#pragma once
#include "geom/Mesh.h"

namespace rs {
namespace prim {

class Quad : public geom::Mesh {
 public:
  Quad(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2,
       const glm::vec2& p3)
      : p0_(p0), p1_(p1), p2_(p2), p3_(p3) {
    vertices_ = {{p0_.x, p0_.y, 0},
                 {p1_.x, p1_.y, 0},
                 {p2_.x, p2_.y, 0},
                 {p3_.x, p3_.y, 0}};
    texcoords_ = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  }

 protected:
  Quad() {}
  glm::vec2 p0_, p1_, p2_, p3_;
  ;
};

class Rectangle : public Quad {
 public:
  Rectangle(const glm::vec2& p0, const glm::vec2& p1)
      : Quad(glm::vec2(p0.x, p0.y), glm::vec2(p1.x, p0.y),
             glm::vec2(p1.x, p1.y), glm::vec2(p0.x, p1.y)) {}

  Rectangle(const glm::vec2& p0, float w, float h)
      : Rectangle(p0, glm::vec2(p0.x + w, p0.y + h)) {}

  Rectangle(float x, float y, float w, float h)
      : Rectangle(glm::vec2(x, y), glm::vec2(x + w, y + h)) {}

  const glm::vec2& getTopLeft() const { return p0_; }
  const glm::vec2& getBottomRight() const { return p1_; }

  float getLeft() const { return p0_.x; }
  float getTop() const { return p0_.y; }
  float getRight() const { return p1_.x; }
  float getBottom() const { return p2_.y; }

  float getWidth() const { return std::abs(p1_.x - p0_.x); }
  float getHeight() const { return std::abs(p2_.y - p1_.y); }

  glm::vec2 getCenter() const {
    return p0_ + glm::vec2(getWidth() * 0.5, getHeight() * 0.5);
  }
};

}  // namespace prim
}  // namespace rs
