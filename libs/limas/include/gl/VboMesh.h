#pragma once
#include "geom/Mesh.h"
#include "gl/Ibo.h"
#include "gl/Shader.h"
#include "gl/Vao.h"

namespace limas {
namespace gl {

template <class V, class N, class C, class T, class I>
class BaseVboMesh : public geom::BaseMesh<V, N, C, T, I> {
 protected:
  void copyFromMesh(const geom::BaseMesh<V, N, C, T, I>& mesh) {
    this->setVertices(mesh.getVertices());
    this->setNormals(mesh.getNormals());
    this->setColors(mesh.getColors());
    this->setTexCoords(mesh.getTexCoords());
    this->setIndices(mesh.getIndices());
    this->update();
  }

 public:
  BaseVboMesh() : geom::BaseMesh<V, N, C, T, I>() {}

  BaseVboMesh(const geom::BaseMesh<V, N, C, T, I>& mesh)
      : BaseVboMesh<V, N, C, T, I>() {
    copyFromMesh(mesh);
  }

  inline BaseVboMesh<V, N, C, T, I>& operator=(
      const geom::BaseMesh<V, N, C, T, I>& rhs) {
    if (this != &rhs) {
      copyFromMesh(rhs);
    }
    return *this;
  }

  void enableVertices() { vao_.bindVbo(V_vbo_, POSITION_ATTRIBUTE, 3); }
  void enableNormals() { vao_.bindVbo(N_vbo_, NORMAL_ATTRIBUTE, 3); }
  void enableColors() { vao_.bindVbo(C_vbo_, COLOR_ATTRIBUTE, 4); }
  void enableTexCoords() { vao_.bindVbo(T_vbo_, TEXCOORD_ATTRIBUTE, 2); }
  void enableIndices() { vao_.bindIbo(ibo_); }

  void disableVertices() { vao_.unbindVbo(POSITION_ATTRIBUTE); }
  void disableNormals() { vao_.unbindVbo(NORMAL_ATTRIBUTE); }
  void disableColors() { vao_.unbindVbo(COLOR_ATTRIBUTE); }
  void disableTexCoords() { vao_.unbindVbo(TEXCOORD_ATTRIBUTE); }
  void disableIndices() { vao_.unbindIbo(); }

  void updateVertices() {
    if (V_vbo_.getSize() != this->getNumVertices()) {
      V_vbo_.allocate(this->vertices_);
      if (!isVertexEnabled()) enableVertices();
    } else {
      V_vbo_.update(this->vertices_, 0);
    }
  }

  void updateNormals() {
    if (N_vbo_.getSize() != this->getNumNormals()) {
      N_vbo_.allocate(this->normals_);
      if (!isNormalEnabled()) enableNormals();
    } else {
      N_vbo_.update(this->normals_, 0);
    }
  }
  void updateColors() {
    if (C_vbo_.getSize() != this->getNumColors()) {
      C_vbo_.allocate(this->colors_);
      if (!isColorEnabled()) enableColors();
    } else {
      C_vbo_.update(this->colors_, 0);
    }
  }
  void updateTexCoords() {
    if (T_vbo_.getSize() != this->getNumTexCoords()) {
      T_vbo_.allocate(this->texcoords_);
      if (!isTexCoordEnabled()) enableTexCoords();
    } else {
      T_vbo_.update(this->texcoords_, 0);
    }
  }
  void updateIndices() {
    if (ibo_.getSize() != this->getNumIndices()) {
      ibo_.allocate(this->indices_);
      if (!isIndexEnabled()) enableIndices();
    } else {
      ibo_.update(this->indices_, 0);
    }
  }

  void update() {
    updateVertices();
    updateNormals();
    updateColors();
    updateTexCoords();
    updateIndices();
  }

  void draw(GLenum mode) const {
    if (this->getNumIndices()) {
      vao_.drawElements(mode, this->getNumIndices());
    } else {
      vao_.drawArrays(mode, 0, this->getNumVertices());
    }
  }

  Vao& getVao() { return vao_; };
  const Vao& getVao() const { return vao_; };

  bool isAttributeEnabled(GLuint index) const {
    return vao_.isAttributeEnabled(index);
  }
  bool isVertexEnabled() const {
    return isAttributeEnabled(POSITION_ATTRIBUTE);
  }
  bool isNormalEnabled() const { return isAttributeEnabled(NORMAL_ATTRIBUTE); }
  bool isColorEnabled() const { return isAttributeEnabled(COLOR_ATTRIBUTE); }
  bool isTexCoordEnabled() const {
    return isAttributeEnabled(TEXCOORD_ATTRIBUTE);
  }
  bool isIndexEnabled() const { return vao_.isIboEnabled(); }

 protected:
  Vao vao_;
  Vbo<V> V_vbo_;
  Vbo<N> N_vbo_;
  Vbo<C> C_vbo_;
  Vbo<T> T_vbo_;
  Ibo<I> ibo_;
};

using VboMesh = BaseVboMesh<glm::vec3, glm::vec3, glm::vec4, glm::vec2, int>;

}  // namespace gl
}  // namespace limas