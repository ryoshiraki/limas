#pragma once
#include "3d/Node.h"
#include "gl/GLUtils.h"

namespace rs {

class Camera : public Node {
 public:
  Camera()
      : Node(),
        fov_(60),
        near_(1e-2),
        far_(INT_MAX),
        lens_offset_(0.f, 0.f, 0.f) {}

  void setupPerspective(float fov, float near, float far) {
    setFov(fov);
    setNearClip(near);
    setFarClip(far);
  }

  void setFov(float fov) { fov_ = fov; }
  float getFov() const { return fov_; }
  void setNearClip(float near) { near_ = near; }
  float getNearClip() const { return near_; }
  void setFarClip(float far) { far_ = far; }
  float getFarClip() const { return far_; }
  void setLensOffset(float x, float y) {
    lens_offset_[0] = x;
    lens_offset_[1] = y;
  }
  glm::vec3 getLensOffset() const { return lens_offset_; }

  glm::vec3 getWorldToScreen(const glm::vec3& p, int x, int y, int w,
                             int h) const {
    glm::vec4 viewport(x, y, w, h);
    auto proj = getProjectionMatrix(x, y, w, h);
    auto view = getModelViewMatrix();
    auto sp = glm::project(p, view, proj, viewport);
    sp[1] = h - sp[1];
    return sp;
  }

  glm::mat4 getModelViewMatrix() const {
    return glm::inverse(getTransformMatrix());
  }

  virtual glm::mat4 getProjectionMatrix(int x, int y, int w, int h) const {
    float aspect = (float)w / (float)h;
    auto projection = glm::perspective(glm::radians(fov_), aspect, near_, far_);
    projection = glm::translate(glm::mat4(1.0),
                                {-lens_offset_.x, -lens_offset_.y, 0.f}) *
                 projection;
    return projection;
  }

  glm::mat4 getProjectionMatrix() const {
    auto viewport = gl::getViewport();
    return getProjectionMatrix(viewport[0], viewport[1], viewport[2],
                               viewport[3]);
  }

  glm::mat4 getModelViewProjectionMatrix(int x, int y, int w, int h) const {
    return getProjectionMatrix(x, y, w, h) * getModelViewMatrix();
  }

  glm::mat4 getModelViewProjectionMatrix() const {
    return getProjectionMatrix() * getModelViewMatrix();
  }

 protected:
  float fov_;
  float near_;
  float far_;
  glm::vec3 lens_offset_;
};

}  // namespace rs
