#pragma once
#include "gl/Context.h"
#include "gl/Rbo.h"
#include "gl/Texture2D.h"
#include "system/Logger.h"

namespace limas {
namespace gl {

class Fbo {
 private:
  struct BufferData {
    GLuint id_;
    const GLsizei width_;
    const GLsizei height_;
    BufferData(GLsizei width, GLsizei height) : width_(width), height_(height) {
      glGenFramebuffers(1, &id_);
    }
    ~BufferData() { glDeleteFramebuffers(1, &id_); }
  };
  std::shared_ptr<BufferData> data_;

 public:
  Fbo() {}
  virtual ~Fbo() {}

  void allocate(GLsizei width, GLsizei height) {
    data_ = std::make_shared<BufferData>(width, height);
  }

  Rbo& attachDepth() {
    rbo_.allocate(getWidth(), getHeight(), GL_DEPTH24_STENCIL8);

    bind();
    rbo_.bind();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, rbo_.getID());
    rbo_.unbind();
    unbind();

    return rbo_;
  }

  Rbo& attachDepthStencil() {
    rbo_.allocate(getWidth(), getHeight(), GL_DEPTH24_STENCIL8);

    bind();
    rbo_.bind();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo_.getID());
    rbo_.unbind();
    unbind();

    return rbo_;
  }

  Texture2D& attachDepthAsTexture() {
    Texture2D tex;
    tex.allocate(getWidth(), getHeight(), GL_DEPTH_COMPONENT24);

    bind();
    tex.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           tex.getID(), 0);
    tex.unbind();
    unbind();
    textures_.push_back(tex);

    return textures_.back();
  }

  Texture2D attachDepthStencilAsTexture() {
    Texture2D tex;
    tex.allocate(getWidth(), getHeight(), GL_DEPTH24_STENCIL8);

    bind();
    tex.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                           GL_TEXTURE_2D, tex.getID(), 0);
    tex.unbind();
    unbind();
    textures_.push_back(tex);

    return textures_.back();
  }

  Texture2D& attachColor(GLenum internal_format) {
    Texture2D tex;
    tex.allocate(getWidth(), getHeight(), internal_format);

    bind();
    tex.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0 + textures_.size(),
                           GL_TEXTURE_2D, tex.getID(), 0);
    attachments_.push_back(GL_COLOR_ATTACHMENT0 + textures_.size());
    tex.unbind();
    unbind();
    textures_.push_back(tex);

    return textures_.back();
  }

  void bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, getID());
    glDrawBuffers(attachments_.size(), &attachments_[0]);

    viewport.push();
    viewport.set(0, getHeight(), getWidth(), -getHeight());
  }

  void unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    viewport.pop();
  }

  bool isCompleted() {
    bind();

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    bool is_completed = status == GL_FRAMEBUFFER_COMPLETE;
    switch (status) {
      case GL_FRAMEBUFFER_COMPLETE:
        logger::error("gl::Fbo") << "COMPLETE" << logger::end();
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        logger::error("gl::Fbo") << "INCOMPLETE_ATTACHMENT" << logger::end();
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        logger::error("gl::Fbo")
            << "INCOMPLETE_MISSING_ATTACHMENT" << logger::end();
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        logger::error("gl::Fbo") << "INCOMPLETE_DRAW_BUFFER" << logger::end();
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        logger::error("gl::Fbo") << "INCOMPLETE_READ_BUFFER" << logger::end();
        break;
        logger::error("gl::Fbo") << "UNKNOWN STATUS" << logger::end();
      default:
        break;
    }

    unbind();

    return is_completed;
  }

  GLuint getID() const { return data_->id_; }
  GLsizei getWidth() const { return data_->width_; }
  GLsizei getHeight() const { return data_->height_; }
  glm::vec2 getSize() const { return glm::vec2(data_->width_, data_->height_); }
  const std::vector<Texture2D>& getTextures() const { return textures_; }
  std::vector<Texture2D>& getTextures() { return textures_; }
  const Texture2D& getTexture(unsigned int i = 0) const {
    return textures_.at(i);
  }
  Texture2D& getTexture(unsigned int i = 0) { return textures_.at(i); }
  Rbo& getRenderBuffer() { return rbo_; }
  const Rbo& getRenderBuffer() const { return rbo_; }

 protected:
  Rbo rbo_;
  std::vector<Texture2D> textures_;
  std::vector<GLuint> attachments_;

 private:
  // GLint viewport[4];
  ViewportState viewport;
};

}  // namespace gl
}  // namespace limas
