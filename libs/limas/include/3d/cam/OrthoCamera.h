#pragma once
#include "3d/cam/BaseCamera.h"

namespace limas {

class OrthoCamera : public BaseCamera {
 public:
  OrthoCamera() : BaseCamera(), zoom_x_(1), zoom_y_(1) {}

  glm::mat4 getProjectionMatrix(float left, float right, float bottom,
                                float top) const override {
    float zx = 1 / zoom_x_;
    float zy = 1 / zoom_y_;
    auto projection =
        glm::ortho(left * zx, right * zx, bottom * zy, top * zy, near_, far_);
    // projection = glm::translate(glm::mat4(1.0f),
    //                             {-lens_offset_.x, -lens_offset_.y, 0.0f}) *
    //              projection;
    return projection;
  }

  void setZoom(float z) { zoom_x_ = zoom_y_ = z; }
  void setZoomX(float z) { zoom_x_ = z; }
  void setZoomY(float z) { zoom_y_ = z; }
  float getZoomX() const { return zoom_x_; }
  float getZoomY() const { return zoom_y_; }

 protected:
  float zoom_x_, zoom_y_;
};

}  // namespace limas
