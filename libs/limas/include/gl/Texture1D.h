#pragma once
#include "gl/Texture2D.h"

namespace limas {
namespace gl {

class Texture1D : public TextureBase {
 public:
  Texture1D() {}

  void allocate(GLsizei width, GLenum internal_format,
                std::optional<GLenum> format = std::nullopt,
                std::optional<GLenum> type = std::nullopt) {
    TextureBase::allocate(GL_TEXTURE_1D, width, 0, 0, internal_format, format,
                          type);
  }

  void loadData(const void* data, GLsizei w = 0, GLsizei x = 0) {
    TextureBase::loadData(data, w, 0, 0, x, 0, 0);
  }
};

}  // namespace gl
}  // namespace limas
