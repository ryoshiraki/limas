#pragma once
#include "system/Logger.h"

namespace limas {
namespace gl {

class Rbo {
 private:
  struct BufferData {
    GLuint id_;
    const GLsizei width_;
    const GLsizei height_;
    const GLenum internal_format_;
    BufferData(GLsizei width, GLsizei height, GLenum internal_format)
        : width_(width), height_(height), internal_format_(internal_format) {
      glGenRenderbuffers(1, &id_);
      glBindRenderbuffer(GL_RENDERBUFFER, id_);
      glRenderbufferStorage(GL_RENDERBUFFER, internal_format_, width_, height_);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    ~BufferData() { glDeleteRenderbuffers(1, &id_); }
  };
  std::shared_ptr<BufferData> data_;

 public:
  Rbo() {};
  virtual ~Rbo() {}

  void allocate(GLsizei width, GLsizei height,
                GLenum internal_format = GL_DEPTH_COMPONENT) {
    data_ = std::make_shared<BufferData>(width, height, internal_format);
  }

  void bind() { glBindRenderbuffer(GL_RENDERBUFFER, data_->id_); }
  void unbind() { glBindRenderbuffer(GL_RENDERBUFFER, 0); }

  GLuint getID() const { return data_->id_; }
  GLsizei getWidth() const { return data_->width_; }
  GLsizei getHeight() const { return data_->height_; }
  glm::vec2 getSize() const { return glm::vec2(data_->width_, data_->height_); }
};

}  // namespace gl
}  // namespace limas
