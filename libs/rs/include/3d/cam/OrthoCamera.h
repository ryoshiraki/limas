#pragma once
#include "3d/cam/Camera.h"

namespace rs {

class OrthoCamera : public Camera {
 public:
  OrthoCamera() : Camera() {}

  glm::mat4 getProjectionMatrix(int x, int y, int w, int h) {
    return glm::translate(glm::mat4(1.0),
                          {-lens_offset.x, -lens_offset.y, 0.f}) *
           glm::ortho(-w / 2.0f, +w / 2.0f, -h / 2.0f, +h / 2.0f, near, far);
  }
};

}  // namespace rs
