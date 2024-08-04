#pragma once
#include "gl/TextureBase.h"

namespace rs {
namespace gl {

class Texture2D : public TextureBase {
 public:
  Texture2D() {}

  void allocate(GLsizei width, GLsizei height, GLenum internal_format,
                std::optional<GLenum> format = std::nullopt,
                std::optional<GLenum> type = std::nullopt) {
    TextureBase::allocate(GL_TEXTURE_2D, width, height, 0, internal_format,
                          format, type);
  }

  void loadData(const void* data, GLsizei w = 0, GLsizei h = 0, GLsizei x = 0,
                GLsizei y = 0) {
    TextureBase::loadData(data, w, h, 0, x, y, 0);
  }

  glm::vec2 getSize() const { return glm::vec2(getWidth(), getHeight()); }
};

}  // namespace gl
}  // namespace rs
