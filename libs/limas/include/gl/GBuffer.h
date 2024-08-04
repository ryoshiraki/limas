#pragma once
#include "gl/Fbo.h"
#include "gl/Shader.h"
#include "system/Logger.h"

namespace limas {
namespace gl {

class GBuffer {
 public:
  enum class Type { Position = 0, Depth, Normal, Albedo };

  void setup(size_t width, size_t height) {
    if (!geom_pass_shader_.isLinked()) {
    }

    if (!light_pass_shader_.isLinked()) {
    }

    fbo_.allocate(width, height);
    textures_[GBuffer::Type::Position] =
        fbo.attachColor(GL_TEXTURE_2D, GL_RGB32F);
    textures_[GBuffer::Type::Depth] = fbo_.attachColor(GL_TEXTURE_2D, GL_R32F);
    textures_[GBuffer::Type::Normal] =
        fbo_.attachColor(GL_TEXTURE_2D, GL_RGB32F);
    textures_[GBuffer::Type::Albedo] = fbo_.attachColor(GL_TEXTURE_2D, GL_RGBA);
    fbo_.attachDepth();

    if (!fbo_.isComplete()) {
      log::error("GBuffer") << "fbo is not completed!" << log::end();
    }
  }

  void bind(const glm::mat4& view, const glm::mat4& proj) {
    fbo.bind();
    geom_pass_shader_.bind();
  }

  void unbind() {
    geom_pass_shader_.unbind();
    fbo.unbind();
  }

  size_t getWidth() const { return fbo.getWidth(); }
  size_t getHeight() const { return fbo.getHeight(); }

  const gl::Texture2D& getPositionTexture() const {
    return textures_[GBuffer::Type::Position];
  }
  const gl::Texture2D& getDepthTexture() const {
    return textures_[GBuffer::Type::Depth];
  }
  const gl::Texture2D& getNormalTexture() const {
    return textures_[GBuffer::Type::Normal];
  }
  const gl::Texture2D& getColorTexture() const {
    return textures_[GBuffer::Type::Albedo];
  }

 protected:
  std::map<GBuffer::Type, gl::Texture2D> textures_;
  gl::Fbo fbo_;
  static gl::Shader geom_pass_shader_;
  static gl::Shader light_pass_shader_;
};

gl::Shader GBuffer::geom_pass_shader_;
gl::Shader GBuffer::light_pass_shader_;

}  // namespace gl
}  // namespace limas