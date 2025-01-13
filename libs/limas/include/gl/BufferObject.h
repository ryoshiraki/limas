#pragma once
#include "system/Logger.h"

namespace limas {
namespace gl {

template <typename T>
class BufferObject {
 private:
  struct BufferData {
    GLuint id_;
    GLsizei count_;
    const GLenum target_;
    const GLsizei stride_;

    BufferData(GLenum target) : target_(target), stride_(sizeof(T)), count_(0) {
      glGenBuffers(1, &id_);
    }
    ~BufferData() { glDeleteBuffers(1, &id_); }

    void allocate(const void* data, GLsizei count, GLenum usage) {
      count_ = count;
      glBindBuffer(target_, id_);
      glBufferData(target_, stride_ * count, data, usage);
      glBindBuffer(target_, 0);
    }

    void update(const void* data, GLsizei count, GLsizei offset) const {
      glBindBuffer(target_, id_);
      glBufferSubData(target_, offset, stride_ * count, data);
      glBindBuffer(target_, 0);
    }
  };
  std::shared_ptr<BufferData> data_;

 public:
  BufferObject() = delete;
  virtual ~BufferObject() {}

  void allocate(const void* data, GLsizei count,
                GLenum usage = GL_DYNAMIC_DRAW) {
    data_->allocate(data, count, GL_DYNAMIC_DRAW);
  }

  void allocate(const std::vector<T>& data, GLenum usage = GL_DYNAMIC_DRAW) {
    allocate(data.data(), data.size(), usage);
  }

  void update(const void* data, GLsizei count, GLsizei offset = 0) const {
    data_->update(data, count, offset);
  }

  void update(const std::vector<T>& data, GLsizei offset = 0) const {
    update(data.data(), data.size(), offset);
  }

  void bind() const { glBindBuffer(getTarget(), getId()); }
  void unbind() const { glBindBuffer(getTarget(), 0); }

  std::vector<T> getData(GLsizei offset, GLsizei count) {
    std::vector<T> data(count);
    bind();
    GLintptr byte_offset = offset * getStride();
    GLsizeiptr byte_length = count * getStride();
    glGetBufferSubData(getTarget(), offset * getStride(), byte_length,
                       data.data());
    unbind();
    return data;
  }

  GLuint getId() const { return data_->id_; }
  GLenum getTarget() const { return data_->target_; }
  GLsizei getStride() const { return data_->stride_; }
  GLsizei getSize() const { return data_->count_; }

 protected:
  BufferObject(GLenum target) : data_(std::make_shared<BufferData>(target)) {}
};

}  // namespace gl
}  // namespace limas
