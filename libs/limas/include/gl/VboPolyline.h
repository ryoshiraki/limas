#pragma once
#include "geom/Polyline.h"
#include "gl/Drawable.h"
#include "gl/Shader.h"
#include "gl/Vao.h"

namespace limas {
namespace gl {

template <class V>
class BaseVboPolyline : public Drawable, public geom::BasePolyline<V> {
 protected:
  void copyFromMesh(const geom::BasePolyline<V>& mesh) {
    this->setVertices(mesh.getVertices());
    this->update();
  }

 public:
  BaseVboPolyline() : Drawable(), geom::BasePolyline<V>() {}

  BaseVboPolyline(const geom::BasePolyline<V>& mesh)
      : Drawable(), geom::BasePolyline<V>() {
    copyFromMesh(mesh);
  }

  inline BaseVboPolyline<V>& operator=(const geom::BasePolyline<V>& rhs) {
    if (this != &rhs) {
      copyFromMesh(rhs);
    }
    return *this;
  }

  void allocateVertices(size_t size) {
    this->vertices_.resize(size);
    updateVertices();
  }

  void updateVertices() {
    if (this->vertices_.empty()) return;
    if (attributes_.find(POSITION_ATTRIBUTE) == attributes_.end()) {
      addAttribute(POSITION_ATTRIBUTE, this->vertices_, 3, GL_FLOAT);
    } else {
      updateAttribute(POSITION_ATTRIBUTE, this->vertices_);
    }
  }

  void update() { updateVertices(); }

  void draw(GLenum mode) const {
    this->Drawable::draw(mode, this->getNumVertices());
  }
};

using VboPolyline = BaseVboPolyline<glm::vec3>;

}  // namespace gl
}  // namespace limas
