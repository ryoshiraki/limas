#pragma once
#include "gl/BufferObject.h"

namespace rs {
namespace gl {

template <typename T>
class Tbo : public BufferObject<T> {
 public:
  Tbo() : BufferObject<T>(GL_TEXTURE_BUFFER) {}
};

}  // namespace gl
}  // namespace rs