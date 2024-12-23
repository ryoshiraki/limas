// #pragma once
// #include "geom/Mesh.h"
// #include "gl/Drawable.h"
// #include "gl/Ibo.h"
// #include "gl/Shader.h"
// #include "gl/Vao.h"

// namespace limas {
// namespace gl {

// template <class V, class N, class C, class T>
// class BaseVboMesh : public Drawable, public geom::BaseMesh<V, N, C, T> {
//  protected:
//   void copyFromMesh(const geom::BaseMesh<V, N, C, T>& mesh) {
//     this->setVertices(mesh.getVertices());
//     this->setNormals(mesh.getNormals());
//     this->setColors(mesh.getColors());
//     this->setTexCoords(mesh.getTexCoords());
//     this->setIndices(mesh.getIndices());
//     this->update();
//   }

//  public:
//   BaseVboMesh() : geom::Mesh() {}

//   BaseVboMesh(const geom::BaseMesh<V, N, C, T>& mesh) : BaseVboMesh() {
//     copyFromMesh(mesh);
//     update();
//   }

//   inline BaseVboMesh& operator=(const geom::BaseMesh<V, N, C, T>& rhs) {
//     if (this != &rhs) {
//       copyFromMesh(rhs);
//     }
//     update();
//     return *this;
//   }

//   void allocateIndices(size_t size) {
//     this->indices_.resize(size);
//     updateIndices();
//   }

//   void allocateVertices(size_t size) {
//     this->vertices_.resize(size);
//     updateVertices();
//   }

//   void allocateTexCoords(size_t size) {
//     this->texcoords_.resize(size);
//     updateTexCoords();
//   }

//   void allocateColors(size_t size) {
//     this->colors_.resize(size);
//     updateColors();
//   }

//   void enableVertices() { enableAttribute(POSITION_ATTRIBUTE); }
//   void enableNormals() { enableAttribute(NORMAL_ATTRIBUTE); }
//   void enableColors() { enableAttribute(COLOR_ATTRIBUTE); }
//   void enableTexCoords() { enableAttribute(TEXCOORD_ATTRIBUTE); }
//   void enableIndices() { vao_.bindIbo(ibo_); }

//   void disableVertices() { disableAttribute(POSITION_ATTRIBUTE); }
//   void disableNormals() { disableAttribute(NORMAL_ATTRIBUTE); }
//   void disableColors() { disableAttribute(COLOR_ATTRIBUTE); }
//   void disableTexCoords() { disableAttribute(TEXCOORD_ATTRIBUTE); }
//   void disableIndices() { vao_.unbindIbo(); }

//   void updateVertices() {
//     auto it = attributes_.find(POSITION_ATTRIBUTE);
//     if (it != attributes_.end()) {
//       auto& attr = it->second;
//       if (this->vertices_.size() == attr->getSize()) {
//         attr->update(this->vertices_.data(), this->vertices_.size());
//       } else {
//         attr->allocate(this->vertices_.data(), this->vertices_.size());
//       }
//     } else {
//       addAttribute(POSITION_ATTRIBUTE, this->vertices_, 3, GL_FLOAT);
//     }
//   }

//   void updateNormals() {
//     auto it = attributes_.find(NORMAL_ATTRIBUTE);
//     if (it != attributes_.end()) {
//       auto& attr = it->second;
//       if (this->normals_.size() == attr->getSize()) {
//         attr->update(this->normals_.data(), this->normals_.size());
//       } else {
//         attr->allocate(this->normals_.data(), this->normals_.size());
//       }
//     } else {
//       addAttribute(NORMAL_ATTRIBUTE, this->normals_, 3, GL_FLOAT);
//     }
//   }

//   void updateColors() {
//     auto it = attributes_.find(COLOR_ATTRIBUTE);
//     if (it != attributes_.end()) {
//       auto& attr = it->second;
//       if (this->colors_.size() == attr->getSize()) {
//         attr->update(this->colors_.data(), this->colors_.size());
//       } else {
//         attr->allocate(this->colors_.data(), this->colors_.size());
//       }
//     } else {
//       addAttribute(COLOR_ATTRIBUTE, this->colors_, 4, GL_FLOAT);
//     }
//   }

//   void updateTexCoords() {
//     auto it = attributes_.find(TEXCOORD_ATTRIBUTE);
//     if (it != attributes_.end()) {
//       auto& attr = it->second;
//       if (this->texcoords_.size() == attr->getSize()) {
//         attr->update(this->texcoords_.data(), this->texcoords_.size());
//       } else {
//         attr->allocate(this->texcoords_.data(), this->texcoords_.size());
//       }
//     } else {
//       addAttribute(TEXCOORD_ATTRIBUTE, this->texcoords_, 2, GL_FLOAT);
//     }
//   }

//   void updateIndices() { this->Drawable::updateIndices(this->indices_); }

//   void update() {
//     updateVertices();
//     updateNormals();
//     updateColors();
//     updateTexCoords();
//     updateIndices();
//   }

//   bool isVertexEnabled() const {
//     return isAttributeEnabled(POSITION_ATTRIBUTE);
//   }
//   bool isNormalEnabled() const { return isAttributeEnabled(NORMAL_ATTRIBUTE);
//   } bool isColorEnabled() const { return isAttributeEnabled(COLOR_ATTRIBUTE);
//   } bool isTexCoordEnabled() const {
//     return isAttributeEnabled(TEXCOORD_ATTRIBUTE);
//   }
//   bool isIndexEnabled() const { return vao_.isIboEnabled(); }
// };

// using VboMesh = BaseVboMesh<glm::vec3, glm::vec3, glm::vec4, glm::vec2>;

// }  // namespace gl
// }  // namespace limas

#pragma once

#include "geom/Mesh.h"
#include "gl/Ibo.h"
#include "gl/Shader.h"
#include "gl/Vao.h"

namespace limas {
namespace gl {

template <class V, class N, class C, class T>
class BaseVboMesh : public geom::BaseMesh<V, N, C, T> {
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
  BaseVboMesh() : geom::BaseMesh<V, N, C, T>() {}

  BaseVboMesh(const geom::BaseMesh<V, N, C, T>& mesh)
      : BaseVboMesh<V, N, C, T>() {
    copyFromMesh(mesh);
    update();
  }

  inline BaseVboMesh<V, N, C, T>& operator=(
      const geom::BaseMesh<V, N, C, T>& rhs) {
    if (this != &rhs) {
      copyFromMesh(rhs);
    }
    update();
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

  void enableVertices() {
    vao_.bindVbo(V_vbo_, POSITION_ATTRIBUTE, 3, GL_FLOAT);
  }
  void enableNormals() { vao_.bindVbo(N_vbo_, NORMAL_ATTRIBUTE, 3, GL_FLOAT); }
  void enableColors() { vao_.bindVbo(C_vbo_, COLOR_ATTRIBUTE, 4, GL_FLOAT); }
  void enableTexCoords() {
    vao_.bindVbo(T_vbo_, TEXCOORD_ATTRIBUTE, 2, GL_FLOAT);
  }
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
      V_vbo_.update(this->vertices_);
    }
  }

  void updateNormals() {
    if (N_vbo_.getSize() != this->getNumNormals()) {
      N_vbo_.allocate(this->normals_);
      if (!isNormalEnabled()) enableNormals();
    } else {
      N_vbo_.update(this->normals_);
    }
  }
  void updateColors() {
    if (C_vbo_.getSize() != this->getNumColors()) {
      C_vbo_.allocate(this->colors_);
      if (!isColorEnabled()) enableColors();
    } else {
      C_vbo_.update(this->colors_);
    }
  }
  void updateTexCoords() {
    if (T_vbo_.getSize() != this->getNumTexCoords()) {
      T_vbo_.allocate(this->texcoords_);
      if (!isTexCoordEnabled()) enableTexCoords();
    } else {
      T_vbo_.update(this->texcoords_);
    }
  }
  void updateIndices() {
    if (ibo_.getSize() != this->getNumIndices()) {
      ibo_.allocate(this->indices_);
      if (!isIndexEnabled()) enableIndices();
    } else {
      ibo_.update(this->indices_);
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

  void drawSubsection(GLenum mode, GLsizei count, GLsizei start = 0) const {
    if (this->getNumIndices()) {
      vao_.drawElements(mode, count);
    } else {
      vao_.drawArrays(mode, start, count);
    }
  }

  void drawInstanced(GLenum mode, int instance_count) const {
    if (this->getNumIndices()) {
      vao_.drawElementsInstanced(mode, this->getNumIndices(), instance_count);
    } else {
      vao_.drawArraysInstanced(mode, 0, this->getNumVertices(), instance_count);
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
  Ibo<GLuint> ibo_;
};

using VboMesh = BaseVboMesh<glm::vec3, glm::vec3, glm::vec4, glm::vec2>;

}  // namespace gl
}  // namespace limas