#pragma once
#include <memory>
#include <optional>

#include "gl/GLUtils.h"
#include "system/Logger.h"
#include "system/Noncopyable.h"

namespace limas {
namespace gl {

class TextureBase {
 protected:
  struct TextureData {
    GLuint id_;
    const GLenum target_;
    const size_t width_;
    const size_t height_;

    const size_t depth_;
    const GLenum internal_format_;
    GLenum format_;
    GLenum type_;
    size_t channels_;

    // for use external texture id
    TextureData(GLuint id, GLenum target, size_t width, size_t height,
                size_t depth, GLenum internal_format,
                std::optional<GLenum> format = std::nullopt,
                std::optional<GLenum> type = std::nullopt)
        : id_(id),
          target_(target),
          width_(width),
          height_(height),
          depth_(depth),
          internal_format_(internal_format) {
      if (!format.has_value()) {
        format_ = getGLFormatFromInternal(internal_format_);
      }

      if (!type.has_value()) {
        type_ = getGLTypeFromInternal(internal_format_);
      }

      channels_ = getNumChannelsFromFormat(format_);
    }

    TextureData(GLenum target, size_t width, size_t height, size_t depth,
                GLenum internal_format,
                std::optional<GLenum> format = std::nullopt,
                std::optional<GLenum> type = std::nullopt)
        : target_(target),
          width_(width),
          height_(height),
          depth_(depth),
          internal_format_(internal_format) {
      if (!format.has_value()) {
        format_ = getGLFormatFromInternal(internal_format_);
      }

      if (!type.has_value()) {
        type_ = getGLTypeFromInternal(internal_format_);
      }

      channels_ = getNumChannelsFromFormat(format_);

      glGenTextures(1, &id_);
      glBindTexture(target_, id_);
      if (target_ == GL_TEXTURE_1D)
        glTexImage1D(target_, 0, internal_format_, width_, 0, format_, type_,
                     0);
      else if (target_ == GL_TEXTURE_2D)
        glTexImage2D(target_, 0, internal_format_, width_, height_, 0, format_,
                     type_, 0);
      else if (target_ == GL_TEXTURE_3D)
        glTexImage3D(target_, 0, internal_format_, width_, height_, depth_, 0,
                     format_, type_, 0);
      glBindTexture(target_, 0);
    }
    ~TextureData() { glDeleteTextures(1, &id_); }
  };

  std::shared_ptr<TextureData> data_;

 public:
  virtual ~TextureBase() {}

  void bind() const { glBindTexture(getTarget(), getId()); }
  void unbind() const { glBindTexture(getTarget(), 0); }

  void setMinFilter(GLint filter) {
    bind();
    glTexParameteri(getTarget(), GL_TEXTURE_MIN_FILTER, filter);
    unbind();
  }

  void setMagFilter(GLint filter) {
    bind();
    glTexParameteri(getTarget(), GL_TEXTURE_MAG_FILTER, filter);
    unbind();
  }

  void setWrapS(GLint wrap) {
    bind();
    glTexParameteri(getTarget(), GL_TEXTURE_WRAP_S, wrap);
    unbind();
  }

  void setWrapT(GLint wrap) {
    bind();
    glTexParameteri(getTarget(), GL_TEXTURE_WRAP_T, wrap);
    unbind();
  }

  void setWrapR(GLint wrap) {
    bind();
    glTexParameteri(getTarget(), GL_TEXTURE_WRAP_R, wrap);
    unbind();
  }

  void setExternalID(GLuint id, GLenum target, size_t width, size_t height,
                     size_t depth, GLenum internal_format,
                     std::optional<GLenum> format = std::nullopt,
                     std::optional<GLenum> type = std::nullopt) {
    if (data_ == nullptr || getId() != id || getWidth() != width ||
        getHeight() != height) {
      data_ = std::make_shared<TextureData>(id, target, width, height, depth,
                                            internal_format, format, type);
    }
  }

  GLuint getId() const { return data_->id_; }
  GLenum getTarget() const { return data_->target_; }
  GLenum getInternalFormat() const { return data_->internal_format_; }
  GLenum getFormat() const { return data_->format_; }
  GLenum getType() const { return data_->type_; }
  size_t getWidth() const { return data_->width_; }
  size_t getHeight() const { return data_->height_; }
  size_t getDepth() const { return data_->depth_; }
  size_t getNumChannels() const { return data_->channels_; }
  bool isAllocated() const { return data_ != nullptr; }

 protected:
  TextureBase() {}

  void allocate(GLenum target, size_t width, size_t height, size_t depth,
                GLenum internal_format,
                std::optional<GLenum> format = std::nullopt,
                std::optional<GLenum> type = std::nullopt) {
    data_ = std::make_shared<TextureData>(target, width, height, depth,
                                          internal_format, format, type);
    setMinFilter(GL_NEAREST);
    setMagFilter(GL_NEAREST);
    setWrapS(GL_CLAMP_TO_EDGE);
    setWrapT(GL_CLAMP_TO_EDGE);
    setWrapR(GL_CLAMP_TO_EDGE);
  }

  void loadData(const void* data, GLsizei w = 0, GLsizei h = 0, GLsizei d = 0,
                GLsizei x = 0, GLsizei y = 0, GLsizei z = 0) {
    bind();
    if (getTarget() == GL_TEXTURE_1D)
      glTexSubImage1D(getTarget(), 0, x, w == 0 ? getWidth() : w, getFormat(),
                      getType(), data);
    else if (getTarget() == GL_TEXTURE_2D)
      glTexSubImage2D(getTarget(), 0, x, y, w == 0 ? getWidth() : w,
                      h == 0 ? getHeight() : h, getFormat(), getType(), data);
    else if (getTarget() == GL_TEXTURE_3D)
      glTexSubImage3D(getTarget(), 0, x, y, z, w == 0 ? getWidth() : w,
                      h == 0 ? getHeight() : h, d == 0 ? getDepth() : d,
                      getFormat(), getType(), data);
    unbind();
  }
};

}  // namespace gl
}  // namespace limas
