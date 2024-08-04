#pragma once
#include "pp/BasePass.h"

namespace rs {
namespace pp {

template <int ITERATIONS = 5>
class Blur : public BasePass {
 public:
  Blur(const std::string& name, size_t width, size_t height) : BasePass(name) {
    shader_ = gl::Shader::create();
    shader_->loadVertex(fs::getCommonResourcesPath() + "shaders/thru.vert");
    switch (ITERATIONS) {
      case 5:
        shader_->loadFragment(fs::getCommonResourcesPath() +
                              "shaders/pp/blur5.frag");
        break;
      case 9:
        shader_->loadFragment(fs::getCommonResourcesPath() +
                              "shaders/pp/blur9.frag");
        break;
      case 13:
        shader_->loadFragment(fs::getCommonResourcesPath() +
                              "shaders/pp/blur13.frag");
        break;
      default:
        log::error("Blur") << "unsupported iterations" << log::end();
        break;
    }
    shader_->link();

    addParam<float>("scale", 0.5);
  }

  virtual void apply(PingPong<gl::Fbo::Ptr>& fbos) override {
    for (int i = 0; i < ITERATIONS; i++) {
      float radius = (ITERATIONS - i - 1) * getParam<float>("scale");
      fbos.getFront()->bind();
      shader_->bind();
      shader_->setUniformTexture("u_tex", fbos.getBack()->getTextures()[0], 0);
      shader_->setUniform2f("u_res", fbos.getFront()->getSize());
      shader_->setUniform2f(
          "u_dir", i % 2 == 0 ? glm::vec2(radius, 0) : glm::vec2(0, radius));
      fill();
      shader_->unbind();
      fbos.getFront()->unbind();
      fbos.swap();
    }
  }

 protected:
  gl::Shader::Ptr shader_;
};

using Blur5 = Blur<5>;
using Blur9 = Blur<9>;
using Blur13 = Blur<13>;

}  // namespace pp
}  // namespace rs