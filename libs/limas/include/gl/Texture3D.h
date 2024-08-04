#pragma once
#include "gl/Texture2D.h"

namespace limas {
namespace gl {

class Texture3D : public TextureBase {
 public:
  Texture3D() {}

  void allocate(GLsizei width, GLsizei height, GLsizei depth,
                GLenum internal_format,
                std::optional<GLenum> format = std::nullopt,
                std::optional<GLenum> type = std::nullopt) {
    TextureBase::allocate(GL_TEXTURE_3D, width, height, depth, internal_format,
                          format, type);
  }

  void loadData(const void* data, GLsizei w = 0, GLsizei h = 0, GLsizei d = 0,
                GLsizei x = 0, GLsizei y = 0, GLsizei z = 0) {
    TextureBase::loadData(data, w, h, d, x, y, z);
  }

  glm::vec3 getSize() const {
    return glm::vec3(getWidth(), getHeight(), getDepth());
  }
};

}  // namespace gl
}  // namespace limas
