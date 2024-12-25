#pragma once
#include "gl/VboMesh.h"

namespace limas {
namespace gl {

template <class V, class N, class C, class T>
class BaseInstancedVboMesh : public gl::BaseVboMesh<V, N, C, T> {
 public:
  BaseInstancedVboMesh() : num_instances_(0), gl::BaseVboMesh<V, N, C, T>() {}

  BaseInstancedVboMesh(const geom::BaseMesh<V, N, C, T>& mesh)
      : BaseVboMesh<V, N, C, T>(mesh) {}

  inline BaseInstancedVboMesh<V, N, C, T>& operator=(
      const geom::BaseMesh<V, N, C, T>& rhs) {
    if (this != &rhs) {
      gl::BaseVboMesh<V, N, C, T>::copyFromMesh(rhs);
    }
    return *this;
  }

  void allocateInstances(size_t num_instance) {
    this->num_instances_ = num_instance;

    // translation_vbo_ = gl::Vbo<glm::vec3>::create();
    // scale_vbo_ = gl::Vbo<glm::vec3>::create();
    // rotation_vbo_ = gl::Vbo<glm::quat>::create();
    // instance_color_vbo_ = gl::Vbo<glm::vec4>::create();

    translations_.resize(num_instance, glm::vec3(1));
    scales_.resize(num_instance, glm::vec3(1));
    rotations_.resize(num_instance, glm::quat(0, 0, 0, 1));
    instance_colors_.resize(num_instance, glm::vec4(1));

    translation_vbo_.allocate(translations_);
    scale_vbo_.allocate(scales_);
    rotation_vbo_.allocate(rotations_);
    instance_color_vbo_.allocate(instance_colors_);

    this->vao_.bindVbo(translation_vbo_, TRANSLATION_ATTRIBUTE, 3, GL_FLOAT);
    this->vao_.bindVbo(rotation_vbo_, ROTATION_ATTRIBUTE, 4, GL_FLOAT);
    this->vao_.bindVbo(scale_vbo_, SCALE_ATTRIBUTE, 3, GL_FLOAT);
    this->vao_.bindVbo(instance_color_vbo_, INSTANCE_COLOR_ATTRIBUTE, 4,
                       GL_FLOAT);

    this->vao_.setVertexAttribDivisor(TRANSLATION_ATTRIBUTE, 1);
    this->vao_.setVertexAttribDivisor(ROTATION_ATTRIBUTE, 1);
    this->vao_.setVertexAttribDivisor(SCALE_ATTRIBUTE, 1);
    this->vao_.setVertexAttribDivisor(INSTANCE_COLOR_ATTRIBUTE, 1);
  }

  void updateTranslations() { translation_vbo_.update(translations_, 0); }
  void updateRotations() { rotation_vbo_.update(rotations_, 0); }
  void updateScales() { scale_vbo_.update(scales_, 0); }
  void updateInstanceColors() {
    instance_color_vbo_.update(instance_colors_, 0);
  }

  void updateInstances() {
    updateTranslations();
    updateRotations();
    updateScales();
    updateInstanceColors();
  }

  void drawInstances(GLenum mode, GLsizei count) const {
    if (this->getNumIndices()) {
      this->vao_.drawElementsInstanced(mode, this->getNumIndices(), count);
    } else {
      this->vao_.drawArraysInstanced(mode, 0, this->getNumVertices(), count);
    }
  }

  size_t getNumInstances() const { return num_instances_; }

  const std::vector<glm::vec3>& getTranslations() const {
    return translations_;
  }
  const std::vector<glm::quat>& getRotations() const { return rotations_; }
  const std::vector<glm::vec3>& getScales() const { return scales_; }
  const std::vector<glm::vec4>& getInstanceColors() const {
    return instance_colors_;
  }

  void setTranslations(const std::vector<glm::vec3>& translations) {
    translations_ = translations;
    translation_vbo_.allocate(translations_);
    this->vao_.bindVbo(translation_vbo_, TRANSLATION_ATTRIBUTE, 3);
    this->vao_.setVertexAttribDivisor(TRANSLATION_ATTRIBUTE, 1);
  }

  void setRotations(const std::vector<glm::quat>& rotations) {
    rotations_ = rotations;
  }
  void setScales(const std::vector<glm::vec3>& scales) { scales_ = scales; }
  void setInstanceColors(const std::vector<glm::vec4>& instance_colors) {
    instance_colors_ = instance_colors;
  }

  std::vector<glm::vec3>& getTranslations() { return translations_; }
  std::vector<glm::quat>& getRotations() { return rotations_; }
  std::vector<glm::vec3>& getScales() { return scales_; }
  std::vector<glm::vec4>& getInstanceColors() { return instance_colors_; }

  size_t getNumTranslations() const { return translations_.size(); }

 protected:
  size_t num_instances_;

  std::vector<glm::vec3> translations_;
  std::vector<glm::quat> rotations_;
  std::vector<glm::vec3> scales_;
  std::vector<glm::vec4> instance_colors_;

  Vbo<glm::vec3> translation_vbo_;
  Vbo<glm::quat> rotation_vbo_;
  Vbo<glm::vec3> scale_vbo_;
  Vbo<glm::vec4> instance_color_vbo_;
};

using InstancedVboMesh =
    BaseInstancedVboMesh<glm::vec3, glm::vec3, glm::vec4, glm::vec2>;

}  // namespace gl
}  // namespace limas