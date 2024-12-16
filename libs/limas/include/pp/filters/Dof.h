#pragma once
#include "pp/BasePass.h"
#include "pp/Gaussian.h"

namespace limas {
namespace pp {

class Dof : public BasePass {
 public:
  Dof(const std::string& name, size_t width, size_t height) : BasePass(name) {
    dof_shader_ = gl::Shader::create();
    dof_shader_->loadVertex(fs::getCommonResourcesPath() + "shaders/thru.vert");
    dof_shader_->loadFragment(fs::getCommonResourcesPath() +
                              "shaders/pp/dof.frag");
    dof_shader_->link();

    auto cam = getParams().addChild("cam");
    cam.addParam<float>("near", 0.1);
    cam.addParam<float>("far", 100.0f);

    auto blur = getParams().addChild("blur");
    blur.addParam<float>("size", 1);
    blur.addParam<float>("falloff", 0);

    auto focus = getParams().addChild("focus");
    focus.addParam<float>("depth", 30.0f);   // m
    focus.addParam<float>("length", 50.0f);  // mm
    focus.addParam<float>("fstop", 2.8f);    // aperture f/2.8, f/4.0
    focus.addParam<float>("coc", 0.03);      // Circle of Confusion 0.03~0.05mm

    auto autofocus = getParams().addChild("autofocus");
    autofocus.addParam<bool>("enable", false);
    autofocus.addParam<glm::vec2>("center", glm::vec2(0.5));

    auto depth_blur = getParams().addChild("depth_blur");
    depth_blur.addParam<bool>("enable", false);
    depth_blur.addParam<float>("size", 1);

    auto manual_dof = getParams().addChild("manual_dof");
    manual_dof.addParam<bool>("enable", false);
    manual_dof.addParam<float>("near_start", 1);
    manual_dof.addParam<float>("near_dist", 1);
    manual_dof.addParam<float>("far_start", 2);
    manual_dof.addParam<float>("far_dist", 1);
  }

  virtual void apply(PingPong<gl::Fbo::Ptr>& fbos) override {
    fbos.getFront()->bind();
    dof_shader_->bind();
    dof_shader_->setUniformTexture("u_color_tex",
                                   fbos.getBack()->getTextures()[0], 0);
    dof_shader_->setUniformTexture("u_depth_tex",
                                   fbos.getBack()->getTextures()[1], 1);
    dof_shader_->setUniform2f("u_res", fbos.getFront()->getSize());

    auto cam = getParams().getChild("cam");
    dof_shader_->setUniform1f("u_near", cam.getValue<float>("near"));
    dof_shader_->setUniform1f("u_far", cam.getValue<float>("far"));

    auto blur = getParams().getChild("blur");
    dof_shader_->setUniform1f("u_blur.size", blur.getValue<float>("size"));
    dof_shader_->setUniform1f("u_blur.falloff",
                              blur.getValue<float>("falloff"));

    auto focus = getParams().getChild("focus");
    dof_shader_->setUniform1f("u_focus.depth", focus.getValue<float>("depth"));
    dof_shader_->setUniform1f("u_focus.length",
                              focus.getValue<float>("length"));
    dof_shader_->setUniform1f("u_focus.fstop", focus.getValue<float>("fstop"));
    dof_shader_->setUniform1f("u_focus.coc", focus.getValue<float>("coc"));

    auto depth_blur = getParams().getChild("depth_blur");
    dof_shader_->setUniform1i("u_depth_blur.enable",
                              depth_blur.getValue<bool>("enable"));
    dof_shader_->setUniform1f("u_depth_blur.size",
                              depth_blur.getValue<float>("size"));

    auto autofocus = getParams().getChild("autofocus");
    dof_shader_->setUniform1i("u_autofocus.enable",
                              autofocus.getValue<bool>("enable"));
    dof_shader_->setUniform2f("u_autofocus.center",
                              autofocus.getValue<glm::vec2>("center"));

    auto manual_dof = getParams().getChild("manual_dof");
    dof_shader_->setUniform1i("u_manual_dof.enable",
                              manual_dof.getValue<bool>("enable"));
    dof_shader_->setUniform1f("u_manual_dof.near_start",
                              manual_dof.getValue<float>("near_start"));
    dof_shader_->setUniform1f("u_manual_dof.near_dist",
                              manual_dof.getValue<float>("near_dist"));
    dof_shader_->setUniform1f("u_manual_dof.far_start",
                              manual_dof.getValue<float>("far_start"));
    dof_shader_->setUniform1f("u_manual_dof.far_dist",
                              manual_dof.getValue<float>("far_dist"));

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
   // #pragma once
   // #include "pp/BasePass.h"
   // #include "pp/Gaussian.h"

// namespace limas {
// namespace pp {

// class Dof : public BasePass {
//  public:
//   Dof(const std::string& name, size_t width, size_t height) {
//     thru_shader_ = gl::Shader::create();
//     thru_shader_->loadVertex(fs::getCommonResourcesPath() +
//                              "shaders/thru.vert");
//     thru_shader_->loadFragment(fs::getCommonResourcesPath() +
//                                "shaders/thru.frag");
//     thru_shader_->link();

//     dof_shader_ = gl::Shader::create();
//     dof_shader_->loadVertex(fs::getCommonResourcesPath() +
//     "shaders/thru.vert");
//     dof_shader_->loadFragment(fs::getCommonResourcesPath() +
//                               "shaders/pp/dof.frag");
//     dof_shader_->link();

//     original_fbo_ = gl::Fbo::create(width, height);
//     original_fbo_->attachColor(GL_RGBA8);
//     original_fbo_->getTexture(0)->setMagFilter(GL_LINEAR);
//     original_fbo_->getTexture(0)->setMinFilter(GL_LINEAR);
//     original_fbo_->getTexture(0)->setWrapS(GL_CLAMP_TO_EDGE);
//     original_fbo_->getTexture(0)->setWrapT(GL_CLAMP_TO_EDGE);

//     setName(name);
//     addParam<float>("aperture", 0.8);
//     addParam<float>("focus", 0.95);
//     addParam<float>("intensity", 0.5);

//     gaussian_ = pp::BasePass::create<pp::Gaussian>("gaussian", width,
//     height);
//   }

//   virtual void apply(PingPong<gl::Fbo::Ptr>& fbos) override {
//     original_fbo_->bind();
//     thru_shader_->bind();
//     thru_shader_->setUniformTexture("u_tex",
//     fbos.getBack()->getTextures()[0],
//                                     0);
//     fill();
//     thru_shader_->unbind();
//     original_fbo_->unbind();

//     gaussian_->apply(fbos);

//     fbos.getFront()->bind();
//     dof_shader_->bind();
//     dof_shader_->setUniformTexture("u_color_tex",
//                                    original_fbo_->getTextures()[0], 0);
//     dof_shader_->setUniformTexture("u_depth_tex",
//                                    fbos.getBack()->getTextures()[1], 1);
//     dof_shader_->setUniform2f("u_res", fbos.getFront()->getSize());
//     dof_shader_->setUniform1f("u_aperture", getParam<float>("aperture"));
//     dof_shader_->setUniform1f("u_focus", getParam<float>("focus"));
//     dof_shader_->setUniform1f("u_intensity", getParam<float>("intensity"));
//     fill();
//     dof_shader_->unbind();
//     fbos.getFront()->unbind();
//     fbos.swap();
//   }

//  protected:
//   pp::BasePass::Ptr gaussian_;
//   gl::Fbo::Ptr original_fbo_;
//   gl::Shader::Ptr thru_shader_;
//   gl::Shader::Ptr dof_shader_;
// };

// }  // namespace pp
// }  // namespace limas