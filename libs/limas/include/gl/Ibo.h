#pragma once
#include "gl/BufferObject.h"

namespace limas {
namespace gl {

template <typename T>
class Ibo : public BufferObject<T> {
 public:
  Ibo() : BufferObject<T>(GL_ELEMENT_ARRAY_BUFFER) {}
};

}  // namespace gl
}  // namespace limas
