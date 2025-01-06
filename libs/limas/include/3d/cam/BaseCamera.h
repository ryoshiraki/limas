#pragma once
#include "3d/Node.h"
#include "gl/GLUtils.h"

namespace limas {

class BaseCamera : public Node {
 public:
  virtual glm::mat4 getProjectionMatrix(float x, float y, float width,
                                        float height) const = 0;

  void setNearClip(float near) { near_ = near; }
  float getNearClip() const { return near_; }
  void setFarClip(float far) { far_ = far; }
  float getFarClip() const { return far_; }
  void setLensOffset(const glm::vec2& offset) { lens_offset_ = offset; }
  glm::vec2 getLensOffset() const { return lens_offset_; }

  glm::vec3 getWorldToScreen(const glm::vec3& p, float x, float y, float w,
                             float h) const {
    glm::vec4 viewport(x, y, w, h);
    auto proj = getProjectionMatrix(x, y, w, h);
    auto view = getModelViewMatrix();
    auto sp = glm::project(p, view, proj, viewport);
    // sp[1] = h - sp[1];
    sp[1] = -sp[1];
    return sp;
  }

  glm::mat4 getModelViewMatrix() const {
    return glm::inverse(getTransformMatrix());
  }

  glm::mat4 getModelViewProjectionMatrix(float x, float y, float w,
                                         float h) const {
    return getProjectionMatrix(x, y, w, h) * getModelViewMatrix();
  }

 protected:
  BaseCamera()
      : Node(), fov_(60), near_(1e-2), far_(INT_MAX), lens_offset_(0.f, 0.f) {}

  float fov_;
  float near_;
  float far_;
  glm::vec2 lens_offset_;
};

}  // namespace limas
