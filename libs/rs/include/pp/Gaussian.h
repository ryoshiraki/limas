#pragma once
#include "pp/BasePass.h"

namespace rs {
namespace pp {

class Gaussian : public BasePass {
 public:
  Gaussian(const std::string& name, size_t width, size_t height) : BasePass(name) {
    blur_shader_ = gl::Shader::create();
    blur_shader_->loadVertex(fs::getCommonResourcesPath() +
                             "shaders/thru.vert");
    blur_shader_->loadFragment(fs::getCommonResourcesPath() +
                               "shaders/pp/gaussian.frag");
    blur_shader_->link();

    blur_fbo_ = gl::Fbo::create(width, height);
    blur_fbo_->attachColor(GL_RGBA8);
    blur_fbo_->getTexture(0)->setMagFilter(GL_LINEAR);
    blur_fbo_->getTexture(0)->setMinFilter(GL_LINEAR);
    blur_fbo_->getTexture(0)->setWrapS(GL_CLAMP_TO_EDGE);
    blur_fbo_->getTexture(0)->setWrapT(GL_CLAMP_TO_EDGE);

    addParam<int>("radius", 6);  // kernel-size = 2 * radius + 1
    addParam<float>("sigma", 3);
  }

  virtual void apply(PingPong<gl::Fbo::Ptr>& fbos) {
    auto weights = computeGaussianWeights(getParam<int>("radius"),
                                          getParam<float>("sigma"));

    blur_fbo_->bind();
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    blur_shader_->bind();
    blur_shader_->setUniformTexture("u_tex", fbos.getBack()->getTextures()[0],
                                    0);
    blur_shader_->setUniform1i("u_horizontal", 0);
    blur_shader_->setUniform2f("u_res_inv",
                               glm::vec2(1.0) / blur_fbo_->getSize());
    blur_shader_->setUniform1i("u_radius", getParam<int>("radius"));
    blur_shader_->setUniform1fv("u_weights", weights.data(), weights.size());
    fill();
    blur_shader_->unbind();
    blur_fbo_->unbind();

    fbos.getFront()->bind();
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    blur_shader_->bind();
    blur_shader_->setUniformTexture("u_tex", blur_fbo_->getTextures()[0], 0);
    blur_shader_->setUniform1i("u_horizontal", 1);
    fill();
    blur_shader_->unbind();
    fbos.getFront()->unbind();
    fbos.swap();
  }

 protected:
  float gaussian(float x, float sigma) {
    return exp(x * x / (-2 * sigma * sigma)) / (sqrt(2 * M_PI) * sigma);
  }

  std::vector<float> computeGaussianWeights(int radius, double sigma) {
    std::vector<float> weights(radius + 1);

    float w = gaussian(0, sigma);
    weights[0] = w;
    float sum = w;
    for (int i = 1; i < weights.size(); i++) {
      float w = gaussian(i, sigma);
      weights[i] = w;
      sum += w * 2;
    }

    for (int i = 0; i < weights.size(); i++) {
      weights[i] /= sum;
    }

    return weights;
  }

  gl::Shader::Ptr blur_shader_;
  gl::Fbo::Ptr blur_fbo_;
};

}  // namespace pp
}  // namespace rs