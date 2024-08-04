#pragma once
#include "pp/BasePass.h"

namespace rs {
namespace pp {

class SSAO : public BasePass {
 public:
  SSAO(const std::string& name, size_t width, size_t height) : BasePass(name) {
    shader_ = gl::Shader::create();
    shader_->loadVertex(fs::getCommonResourcesPath() + "shaders/thru.vert");
    shader_->loadFragment(fs::getCommonResourcesPath() +
                          "shaders/pp/ssao.frag");
    shader_->link();
  }

  virtual void apply(PingPong<gl::Fbo::Ptr>& fbos) override {
    fbos.getFront()->bind();
    shader_->bind();
    shader_->setUniformTexture("u_tex", fbos.getBack()->getTextures()[0], 0);
    shader_->setUniform2f("u_res", fbos.getFront()->getSize());
    fill();
    shader_->unbind();
    fbos.getFront()->unbind();
    fbos.swap();
  }

 protected:
  gl::Shader::Ptr shader_;
};

}  // namespace pp
}  // namespace rs