#pragma once
#include "gl/Tbo.h"
#include "gl/TextureBase.h"

namespace rs {
namespace gl {

class TextureBuffer : public TextureBase {
 public:
  TextureBuffer() {}

  template <typename T>
  void bindTbo(const Tbo<T>& tbo, GLenum internal_format) {
    TextureBase::allocate(GL_TEXTURE_BUFFER, 1, 1, 1, internal_format);

    bind();
    tbo.bind();
    glTexBuffer(GL_TEXTURE_BUFFER, internal_format, tbo.getID());
    tbo.unbind();
    unbind();
  }
};

}  // namespace gl
}  // namespace rs
