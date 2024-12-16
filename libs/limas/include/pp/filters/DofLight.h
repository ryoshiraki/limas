#pragma once
#include "pp/BasePass.h"
#include "pp/Gaussian.h"

namespace limas {
namespace pp {

class DofLight : public BasePass {
 public:
  DofLight(const std::string& name, size_t width, size_t height)
      : BasePass(name) {
    dof_shader_ = gl::Shader::create();
    dof_shader_->loadVertex(fs::getCommonResourcesPath() + "shaders/thru.vert");
    dof_shader_->loadFragment(fs::getCommonResourcesPath() +
                              "shaders/pp/dof_light.frag");
    dof_shader_->link();
  }

  virtual void apply(PingPong<gl::Fbo::Ptr>& fbos) override {
    fbos.getFront()->bind();
    dof_shader_->bind();
    dof_shader_->setUniformTexture("u_color_tex",
                                   fbos.getBack()->getTextures()[0], 0);
    dof_shader_->setUniformTexture("u_depth_tex",
                                   fbos.getBack()->getTextures()[1], 1);
    dof_shader_->setUniform2f("u_res", fbos.getFront()->getSize());
    fill();
    dof_shader_->unbind();
    fbos.getFront()->unbind();
    fbos.swap();
  }

 protected:
  gl::Shader::Ptr dof_shader_;
};

}  // namespace pp
}  // namespace limas
