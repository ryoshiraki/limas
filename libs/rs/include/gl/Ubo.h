#pragma once
#include "gl/BufferObject.h"

namespace rs {
namespace gl {

template <typename T>
class Ubo : public BufferObject<T> {
 public:
  Ubo() : BufferObject<T>(GL_UNIFORM_BUFFER) {}

  void bindBufferBase(GLuint index) const {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, BufferObject<T>::getID());
  }

  void unbindBufferBase(GLuint index) const {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, BufferObject<T>::getID());
  }
};

}  // namespace gl
}  // namespace rs