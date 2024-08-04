#pragma once
#include "3d/Light.h"
#include "gl/GLUtils.h"

namespace rs {

class DirectionalLight : public Light {
 public:
  DirectionalLight() : Light() {}
};

}  // namespace rs
