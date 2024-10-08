#pragma once
#include "gl/BufferObject.h"

namespace limas {
namespace gl {

template <typename T>
class Vbo : public BufferObject<T> {
 public:
  Vbo() : BufferObject<T>(GL_ARRAY_BUFFER) {}
};

}  // namespace gl
}  // namespace limas
