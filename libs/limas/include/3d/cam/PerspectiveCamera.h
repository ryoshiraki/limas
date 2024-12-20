#pragma once
#include "3d/cam/BaseCamera.h"
#include "gl/GLUtils.h"

namespace limas {

class PerspectiveCamera : public BaseCamera {
 public:
  PerspectiveCamera() : BaseCamera(), fov_(60) {}

  void setFov(float fov) { fov_ = fov; }
  float getFov() const { return fov_; }

  glm::mat4 getProjectionMatrix(float x, float y, float width,
                                float height) const override {
    float aspect = width / height;
    glm::mat4 projection =
        glm::perspective(glm::radians(fov_), aspect, near_, far_);
    projection = glm::translate(glm::mat4(1.0f),
                                {-lens_offset_.x, -lens_offset_.y, 0.0f}) *
                 projection;
    return projection;
  }

  glm::mat4 getProjectionMatrixWithFOVs(float fov_x, float fov_y) const {
    float tan_half_fov_x = tan(glm::radians(fov_x) / 2.0f);
    float tan_half_fov_y = tan(glm::radians(fov_y) / 2.0f);

    glm::mat4 projection(0.0f);
    projection[0][0] = 1.0f / tan_half_fov_x;
    projection[1][1] = 1.0f / tan_half_fov_y;
    projection[2][2] = -(far_ + near_) / (far_ - near_);
    projection[2][3] = -1.0f;
    projection[3][2] = -(2.0f * far_ * near_) / (far_ - near_);

    projection = glm::translate(glm::mat4(1.0f),
                                {-lens_offset_.x, -lens_offset_.y, 0.0f}) *
                 projection;

    return projection;
  }

  glm::vec3 getWorldToScreenWithFOVs(const glm::vec3& p, int x, int y, int w,
                                     int h, float fov_x, float fov_y) const {
    glm::vec4 viewport(x, y, w, h);
    auto proj = getProjectionMatrixWithFOVs(fov_x, fov_y);
    auto view = getModelViewMatrix();
    auto sp = glm::project(p, view, proj, viewport);
    sp[1] = h - sp[1];
    return sp;
  }

 protected:
  float fov_;
};

}  // namespace limas
