#pragma once
#include "geom/Polyline.h"
#include "gl/Shader.h"
#include "gl/Vao.h"

namespace limas {
namespace gl {

template <class V>
class BaseVboPolyline : public geom::BasePolyline<V> {
 public:
  BaseVboPolyline() : geom::BasePolyline<V>() {}

  BaseVboPolyline(const geom::BasePolyline<V>& poly) : BaseVboPolyline<V>() {
    this->setPositions(poly.getPositions());
    this->update();
  }

  inline BaseVboPolyline<V>& operator=(const geom::BasePolyline<V>& rhs) {
    if (this != &rhs) {
      this->setPositions(rhs.getPositions());
      this->update();
    }
    return *this;
  }

  void enablePositions() { vao_.bindVbo(vbo_, POSITION_ATTRIBUTE, 3); }
  void disablePositions() { vao_.unbindVbo(POSITION_ATTRIBUTE); }

  void update() {
    if (this->getNumPositions() == 0) return;

    if (vbo_->getSize() != this->getNumPositions()) {
      vbo_->allocate(this->Positions_);
      if (!isVertexEnabled()) enablePositions();
    } else {
      vbo_->update(this->Positions_, 0);
    }
  }

  void draw(GLenum mode) const {
    vao_.drawArrays(mode, 0, this->getNumPositions());
  }

  Vao& getVao() { return vao_; };
  const Vao& getVao() const { return vao_; };

  bool isAttributeEnabled(GLuint index) const {
    return vao_.isAttributeEnabled(index);
  }

  bool isVertexEnabled() const {
    return isAttributeEnabled(POSITION_ATTRIBUTE);
  }

 protected:
  Vao vao_;
  Vbo<V> vbo_;
};

using VboPolyline = BaseVboPolyline<glm::vec3>;

}  // namespace gl
}  // namespace limas
