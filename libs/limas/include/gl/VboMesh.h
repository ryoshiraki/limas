#pragma once

#include "geom/Mesh.h"
#include "gl/Drawable.h"
#include "gl/Ibo.h"
#include "gl/Shader.h"
#include "gl/Vao.h"

namespace limas {
namespace gl {

template <class V, class N, class C, class T>
class BaseVboMesh : public Drawable, public geom::BaseMesh<V, N, C, T> {
 protected:
  void copyFromMesh(const geom::BaseMesh<V, N, C, T>& mesh) {
    this->setVertices(mesh.getVertices());
    this->setNormals(mesh.getNormals());
    this->setColors(mesh.getColors());
    this->setTexCoords(mesh.getTexCoords());
    this->setIndices(mesh.getIndices());
    this->update();
  }

 public:
  BaseVboMesh() : Drawable(), geom::BaseMesh<V, N, C, T>() {}

  BaseVboMesh(const geom::BaseMesh<V, N, C, T>& mesh)
      : Drawable(), geom::BaseMesh<V, N, C, T>() {
    copyFromMesh(mesh);
  }

  inline BaseVboMesh<V, N, C, T>& operator=(
      const geom::BaseMesh<V, N, C, T>& rhs) {
    if (this != &rhs) {
      copyFromMesh(rhs);
    }
    return *this;
  }

  void allocateIndices(size_t size) {
    this->indices_.resize(size);
    updateIndices();
  }

  void allocateVertices(size_t size) {
    this->vertices_.resize(size);
    updateVertices();
  }

  void allocateTexCoords(size_t size) {
    this->texcoords_.resize(size);
    updateTexCoords();
  }

  void allocateColors(size_t size) {
    this->colors_.resize(size);
    updateColors();
  }

  void enableVertices() { enableAttribute(POSITION_ATTRIBUTE); }
  void enableNormals() { enableAttribute(NORMAL_ATTRIBUTE); }
  void enableColors() { enableAttribute(COLOR_ATTRIBUTE); }
  void enableTexCoords() { enableAttribute(TEXCOORD_ATTRIBUTE); }
  void enableIndices() { vao_.bindIbo(ibo_); }

  void disableVertices() { disableAttribute(POSITION_ATTRIBUTE); }
  void disableNormals() { disableAttribute(NORMAL_ATTRIBUTE); }
  void disableColors() { disableAttribute(COLOR_ATTRIBUTE); }
  void disableTexCoords() { disableAttribute(TEXCOORD_ATTRIBUTE); }
  void disableIndices() { vao_.unbindIbo(); }

  void updateVertices() {
    if (this->vertices_.empty()) return;
    if (attributes_.find(POSITION_ATTRIBUTE) == attributes_.end()) {
      addAttribute(POSITION_ATTRIBUTE, this->vertices_, 3, GL_FLOAT);
    } else {
      updateAttribute(POSITION_ATTRIBUTE, this->vertices_);
    }
  }

  void updateNormals() {
    if (this->normals_.empty()) return;
    if (attributes_.find(NORMAL_ATTRIBUTE) == attributes_.end()) {
      addAttribute(NORMAL_ATTRIBUTE, this->normals_, 3, GL_FLOAT);
    } else if (this->normals_.size()) {
      updateAttribute(NORMAL_ATTRIBUTE, this->normals_);
    }
  }

  void updateColors() {
    if (this->colors_.empty()) return;
    if (attributes_.find(COLOR_ATTRIBUTE) == attributes_.end()) {
      addAttribute(COLOR_ATTRIBUTE, this->colors_, 4, GL_FLOAT);
    } else {
      updateAttribute(COLOR_ATTRIBUTE, this->colors_);
    }
  }

  void updateTexCoords() {
    if (this->texcoords_.empty()) return;
    if (attributes_.find(TEXCOORD_ATTRIBUTE) == attributes_.end()) {
      addAttribute(TEXCOORD_ATTRIBUTE, this->texcoords_, 2, GL_FLOAT);
    } else {
      updateAttribute(TEXCOORD_ATTRIBUTE, this->texcoords_);
    }
  }

  void updateIndices() { this->Drawable::updateIndices(this->indices_); }

  void update() {
    updateVertices();
    updateNormals();
    updateColors();
    updateTexCoords();
    updateIndices();
  }

  void draw(GLenum mode) const {
    if (isIndexEnabled()) {
      this->Drawable::draw(mode, this->getNumIndices());
    } else {
      this->Drawable::draw(mode, this->getNumVertices());
    }
  }

  void drawInstanced(GLenum mode, GLsizei instance_count) const {
    if (isIndexEnabled()) {
      this->Drawable::drawInstanced(mode, instance_count,
                                    this->getNumIndices());
    } else {
      this->Drawable::drawInstanced(mode, instance_count,
                                    this->getNumVertices());
    }
  }

  bool isVertexEnabled() const {
    return isAttributeEnabled(POSITION_ATTRIBUTE);
  }
  bool isNormalEnabled() const { return isAttributeEnabled(NORMAL_ATTRIBUTE); }
  bool isColorEnabled() const { return isAttributeEnabled(COLOR_ATTRIBUTE); }
  bool isTexCoordEnabled() const {
    return isAttributeEnabled(TEXCOORD_ATTRIBUTE);
  }
};

using VboMesh = BaseVboMesh<glm::vec3, glm::vec3, glm::vec4, glm::vec2>;

}  // namespace gl
}  // namespace limas