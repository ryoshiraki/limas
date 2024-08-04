#pragma once
#include "gl/Fbo.h"
#include "gl/GLUtils.h"
#include "graphics/Pixels.h"
#include "system/Logger.h"

namespace limas {
namespace gl {

class PboPacker {
 private:
  struct BufferData {
    std::array<GLuint, 2> ids_;
    const size_t width_;
    const size_t height_;
    const GLenum internal_format_;
    GLenum format_;
    GLenum type_;
    size_t channels_;

    BufferData(size_t width, size_t height, GLenum internal_format)
        : width_(width), height_(height), internal_format_(internal_format) {
      format_ = getGLFormatFromInternal(internal_format_);
      type_ = getGLTypeFromInternal(internal_format_);
      channels_ = getNumChannelsFromFormat(format_);

      glGenBuffers(2, ids_.data());
      for (int i = 0; i < 2; i++) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, ids_[i]);
        glBufferData(GL_PIXEL_PACK_BUFFER,
                     width_ * height_ * getByteOfPixel(internal_format_), 0,
                     GL_STREAM_READ);
      }
      glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }

    ~BufferData() { glDeleteBuffers(2, ids_.data()); }
  };

 public:
  PboPacker() : flag_(0), b_async_(false) {}
  virtual ~PboPacker() {}

  void allocate(size_t width, size_t height, GLenum internal_format) {
    data_ = std::make_shared<BufferData>(width, height, internal_format);
  }

  void enableAsync() { b_async_ = true; }
  void disableAsync() { b_async_ = false; }
  bool isAsync() const { return b_async_; }

  template <typename T>
  bool readTo(std::vector<T>* data, GLuint fbo_id, int attachment_id = 0) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_id);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, data_->ids_[flag_]);
    glReadPixels(0, 0, data_->width_, data_->height_, data_->format_,
                 data_->type_, nullptr);

    if (b_async_)
      flag_ = (flag_ + 1) % 2;
    else
      flag_ = 0;

    glBindBuffer(GL_PIXEL_PACK_BUFFER, data_->ids_[flag_]);

    T* ptr = (T*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    if (ptr != nullptr) {
      std::copy(ptr, ptr + data->size(), data->begin());
      glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return (ptr != nullptr);
  }

 private:
  std::shared_ptr<BufferData> data_;
  uint flag_;
  bool b_async_;
};

class PboUnpacker {
 private:
  struct BufferData {
    std::array<GLuint, 2> ids_;
    const size_t width_;
    const size_t height_;
    const GLenum internal_format_;
    GLenum format_;
    GLenum type_;
    size_t channels_;

    BufferData(size_t width, size_t height, GLenum internal_format)
        : width_(width), height_(height), internal_format_(internal_format) {
      format_ = getGLFormatFromInternal(internal_format_);
      type_ = getGLTypeFromInternal(internal_format_);
      channels_ = getNumChannelsFromFormat(format_);

      glGenBuffers(2, ids_.data());
      for (int i = 0; i < 2; i++) {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ids_[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER,
                     width_ * height_ * getByteOfPixel(internal_format_), 0,
                     GL_STREAM_DRAW);
      }
      glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }

    ~BufferData() { glDeleteBuffers(2, ids_.data()); }
  };

 public:
  PboUnpacker() : flag_(0), b_async_(false) {}
  virtual ~PboUnpacker() {}

  void allocate(size_t width, size_t height, GLenum internal_format) {
    data_ = std::make_shared<BufferData>(width, height, internal_format);
  }

  void enableAsync() { b_async_ = true; }
  void disableAsync() { b_async_ = false; }
  bool isAsync() const { return b_async_; }

  template <typename T>
  bool setFromPixels(GLuint tex_id, std::vector<T>& data, GLsizei width,
                     GLsizei height, GLsizei offset_x = 0,
                     GLsizei offset_y = 0) {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, data_->ids_[flag_]);

    T* ptr = (T*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    if (!ptr) return false;
    T* front = &data[0];
    std::copy(front, front + width * height * data_->channels_, ptr);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    if (b_async_)
      flag_ = (flag_ + 1) % 2;
    else
      flag_ = 0;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, data_->ids_[flag_]);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, offset_x, offset_y, width, height,
                    data_->format_, data_->type_, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    return true;
  }

 private:
  std::shared_ptr<BufferData> data_;
  uint flag_;
  bool b_async_;
};

}  // namespace gl
}  // namespace limas