#pragma once
#include "3d/Light.h"
#include "gl/GLUtils.h"

namespace limas {

class DirectionalLight : public Light {
 public:
  DirectionalLight() : Light() {}
};

}  // namespace limas
