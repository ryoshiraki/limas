#pragma once
#include "geom/Polyline.h"
#include "gl/Shader.h"
#include "gl/Vao.h"

namespace limas {
namespace gl {

template <class V>
class BaseVboPolyline : public geom::BasePolyline<V> {
 protected:
  void copyFromPolyline(const geom::BasePolyline<V>& polyline) {
    this->setVertices(polyline.getVertices());
    this->update();
  }

 public:
  BaseVboPolyline() : geom::BasePolyline<V>() {}

  BaseVboPolyline(const geom::BasePolyline<V>& polyline)
      : BaseVboPolyline<V>() {
    this->copyFromPolyline(polyline);
  }

  inline BaseVboPolyline<V>& operator=(const geom::BasePolyline<V>& rhs) {
    if (this != &rhs) {
      this->copyFromPolyline(rhs);
    }
    return *this;
  }

  void enableVertices() { vao_.bindVbo(vbo_, POSITION_ATTRIBUTE, 3); }
  void disableVertices() { vao_.unbindVbo(POSITION_ATTRIBUTE); }

  void update() {
    if (this->getNumVertices() == 0) return;
    if (vbo_.getSize() != this->getNumVertices()) {
      vbo_.allocate(this->vertices_);
      if (!isVertexEnabled()) enableVertices();
    } else {
      vbo_.update(this->vertices_, 0);
    }
  }

  void draw(GLenum mode) const {
    vao_.drawArrays(mode, 0, this->getNumVertices());
  }

  void drawSubsection(GLenum mode, GLsizei count, GLsizei start = 0) const {
    vao_.drawArrays(mode, start, count);
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
