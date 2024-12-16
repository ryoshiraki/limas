#pragma once
#include "pp/BasePass.h"

namespace limas {
namespace pp {

class Fxaa : public BasePass {
 public:
  Fxaa(const std::string& name, size_t width, size_t height) : BasePass(name) {
    shader_ = gl::Shader::create();
    shader_->loadVertex(fs::getCommonResourcesPath() + "shaders/thru.vert");
    shader_->loadFragment(fs::getCommonResourcesPath() +
                          "shaders/pp/fxaa.frag");
    shader_->link();
  }

  virtual void apply(PingPong<gl::Fbo::Ptr>& fbos) override {
    fbos.getFront()->bind();
    shader_->bind();
    shader_->setUniformTexture("u_tex", fbos.getBack()->getTexture(0), 0);
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
}  // namespace limas