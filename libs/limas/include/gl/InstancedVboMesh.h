#pragma once
#include "gl/VboMesh.h"

namespace limas {
namespace gl {

template <class V, class N, class C, class T>
class BaseInstancedVboMesh : public BaseVboMesh<V, N, C, T> {
 public:
  BaseInstancedVboMesh() : BaseVboMesh<V, N, C, T>() {}

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
    translations_.resize(num_instance, glm::vec3(0));
    scales_.resize(num_instance, glm::vec3(1));
    rotations_.resize(num_instance, glm::quat(0, 0, 0, 1));
    instance_colors_.resize(num_instance, glm::vec4(1));
    updateInstances();
  }

  void updateTranslations() {
    if (this->translations_.empty()) return;
    if (this->attributes_.find(TRANSLATION_ATTRIBUTE) ==
        this->attributes_.end()) {
      this->addAttribute(TRANSLATION_ATTRIBUTE, this->translations_, 3,
                         GL_FLOAT);
      this->vao_.setVertexAttribDivisor(TRANSLATION_ATTRIBUTE, 1);
    } else {
      this->updateAttribute(TRANSLATION_ATTRIBUTE, this->translations_);
    }
  }

  void updateRotations() {
    if (this->rotations_.empty()) return;
    if (this->attributes_.find(ROTATION_ATTRIBUTE) == this->attributes_.end()) {
      this->addAttribute(ROTATION_ATTRIBUTE, this->rotations_, 4, GL_FLOAT);
      this->vao_.setVertexAttribDivisor(ROTATION_ATTRIBUTE, 1);
    } else {
      this->updateAttribute(ROTATION_ATTRIBUTE, this->rotations_);
    }
  }

  void updateScales() {
    if (this->scales_.empty()) return;
    if (this->attributes_.find(SCALE_ATTRIBUTE) == this->attributes_.end()) {
      this->addAttribute(SCALE_ATTRIBUTE, this->scales_, 3, GL_FLOAT);
      this->vao_.setVertexAttribDivisor(SCALE_ATTRIBUTE, 1);
    } else {
      this->updateAttribute(SCALE_ATTRIBUTE, this->scales_);
    }
  }

  void updateInstanceColors() {
    if (this->instance_colors_.empty()) return;
    if (this->attributes_.find(INSTANCE_COLOR_ATTRIBUTE) ==
        this->attributes_.end()) {
      this->addAttribute(INSTANCE_COLOR_ATTRIBUTE, this->instance_colors_, 4,
                         GL_FLOAT);
      this->vao_.setVertexAttribDivisor(INSTANCE_COLOR_ATTRIBUTE, 1);
    } else {
      this->updateAttribute(INSTANCE_COLOR_ATTRIBUTE, this->instance_colors_);
    }
  }

  void updateInstances() {
    updateTranslations();
    updateRotations();
    updateScales();
    updateInstanceColors();
  }

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
  size_t getNumRotations() const { return rotations_.size(); }
  size_t getNumScales() const { return scales_.size(); }
  size_t getNumInstanceColors() const { return instance_colors_.size(); }

 protected:
  std::vector<glm::vec3> translations_;
  std::vector<glm::quat> rotations_;
  std::vector<glm::vec3> scales_;
  std::vector<glm::vec4> instance_colors_;
};

using InstancedVboMesh =
    BaseInstancedVboMesh<glm::vec3, glm::vec3, glm::vec4, glm::vec2>;

}  // namespace gl
}  // namespace limas
