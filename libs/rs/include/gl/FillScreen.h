#pragma once
#include "gl/VboMesh.h"
#include "primitives/Rectangle.h"
#include "system/Singleton.h"

namespace rs {
namespace gl {
class FillScreen {
  friend class Singleton<FillScreen>;
public:
  void render() const { rectangle_.draw(GL_TRIANGLE_FAN); }

 private:
  FillScreen() { rectangle_ = prim::Rectangle(-1, -1, 2, 2); }
  gl::VboMesh rectangle_;
};

inline void fillScreen() {
  return Singleton<FillScreen>::getInstance().render();
}

}  // namespace gl
}  // namespace rs