#pragma once
#include "pp/BasePass.h"

namespace limas {
namespace pp {

class Brcosa : public BasePass {
 public:
  Brcosa(const std::string& name, size_t width, size_t height)
      : BasePass(name) {
    shader_ = gl::Shader::create();
    shader_->loadVertex(fs::getCommonResourcesPath() + "shaders/thru.vert");
    shader_->loadFragment(fs::getCommonResourcesPath() +
                          "shaders/pp/brcosa.frag");
    shader_->link();

    addParam<float>("brightness", 1);
    addParam<float>("contrast", 1);
    addParam<float>("saturation", 1);
    addParam<float>("invert", 0);
    addParam<float>("alpha", 1);
  }

  virtual void apply(PingPong<gl::Fbo::Ptr>& fbos) {
    fbos.getFront()->bind();
    shader_->bind();
    shader_->setUniformTexture("u_tex", fbos.getBack()->getTexture(0), 0);
    shader_->setUniform2f("u_res", fbos.getFront()->getSize());
    shader_->setUniform1f("u_brightness", getParam<float>("brightness"));
    shader_->setUniform1f("u_contrast", getParam<float>("contrast"));
    shader_->setUniform1f("u_saturation", getParam<float>("saturation"));
    shader_->setUniform1f("u_invert", getParam<float>("invert"));
    shader_->setUniform1f("u_alpha", getParam<float>("alpha"));
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