#pragma once
#include "math/Math.h"

namespace rs {
namespace gl {

template <typename StateType>
class State {
 public:
  using Ptr = std::unique_ptr<State>;

  void push() {
    StateType state = get();
    states_.push(state);
  }
  void push(StateType state) { states_.push(state); }

  void pop() {
    if (states_.empty()) return;
    StateType state = states_.top();
    states_.pop();
    set(state);
  }

  virtual StateType get() = 0;
  virtual void set(StateType state) = 0;

 private:
  std::stack<StateType> states_;
};

template <GLenum cap>
class TestState : public State<GLboolean> {
 public:
  GLboolean get() override { return glIsEnabled(cap); }
  void set(GLboolean state) override {
    if (state)
      glEnable(cap);
    else
      glDisable(cap);
  }
};

using DepthTestState = TestState<GL_DEPTH_TEST>;
using BlendState = TestState<GL_BLEND>;
using CullFaceState = TestState<GL_CULL_FACE>;
using StencilTestState = TestState<GL_STENCIL_TEST>;
using ScissorTestState = TestState<GL_SCISSOR_TEST>;

#pragma mark DEPTH
class DepthFuncState : public State<GLenum> {
 public:
  GLenum get() override {
    GLenum mode;
    glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&mode);
    return mode;
  }
  void set(GLenum state) override { glDepthFunc(state); }
};

class DepthMaskState : public State<GLboolean> {
 public:
  GLboolean get() override {
    GLboolean mode;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &mode);
    return mode;
  }
  void set(GLboolean state) override { glDepthMask(state); }
};

class DepthRangeState : public State<std::array<GLdouble, 2>> {
 public:
  std::array<GLdouble, 2> get() override {
    std::array<GLdouble, 2> state;
    glGetDoublev(GL_DEPTH_RANGE, state.data());
    return state;
  }
  void set(std::array<GLdouble, 2> state) override {
    glDepthRange(state[0], state[1]);
  }
};

#pragma mark BLEND

class BlendFuncSeparateState : public State<std::array<GLenum, 4>> {
 public:
  std::array<GLenum, 4> get() override {
    std::array<GLenum, 4> state;
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&state[0]);
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&state[1]);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&state[2]);
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&state[3]);
    return state;
  }
  void set(std::array<GLenum, 4> state) override {
    glBlendFuncSeparate(state[0], state[1], state[2], state[3]);
  }

  void set(GLenum src, GLenum dst, GLenum src_alpha, GLenum dst_alpha) {
    set({src, dst, src_alpha, dst_alpha});
  }

  void set(GLenum src, GLenum dst) { set({src, dst, src, dst}); }

  void setBlendAlpha() { set(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
  void setBlendAdd() { set(GL_SRC_ALPHA, GL_ONE); }
  void setBlendMulti() { set(GL_ZERO, GL_SRC_COLOR); }
  void setBlendScreen() { set(GL_ONE_MINUS_DST_COLOR, GL_ONE); }
  void setBlendReverse() { set(GL_ONE_MINUS_DST_COLOR, GL_ZERO); }
  void setBlendReverse2() {
    set(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
  }
  void setBlendSeparatedAlpha() {
    set(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);
  }
};

#pragma mark CULLING
class CullFaceModeState : public State<GLenum> {
 public:
  GLenum get() override {
    GLenum mode;
    glGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&mode);
    return mode;
  }
  void set(GLenum state) override { glCullFace(state); }
};

class FrontFaceState : public State<GLenum> {
 public:
  GLenum get() override {
    GLenum mode;
    glGetIntegerv(GL_FRONT_FACE, (GLint*)&mode);
    return mode;
  }
  void set(GLenum state) override { glFrontFace(state); }
};

class StencilFuncState : public State<std::tuple<GLenum, GLint, GLuint>> {
 public:
  std::tuple<GLenum, GLint, GLuint> get() override {
    std::tuple<GLenum, GLint, GLuint> state;
    glGetIntegerv(GL_STENCIL_FUNC, (GLint*)&std::get<0>(state));
    glGetIntegerv(GL_STENCIL_FAIL, (GLint*)&std::get<1>(state));
    glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint*)&std::get<2>(state));
    return state;
  }
  void set(std::tuple<GLenum, GLint, GLuint> state) override {
    glStencilFunc(std::get<0>(state), std::get<1>(state), std::get<2>(state));
  }

  void set(GLenum func, GLint ref, GLuint mask) { set({func, ref, mask}); }
};

class StencilOpState : public State<std::tuple<GLenum, GLenum, GLenum>> {
 public:
  std::tuple<GLenum, GLenum, GLenum> get() override {
    std::tuple<GLenum, GLenum, GLenum> state;
    glGetIntegerv(GL_STENCIL_FAIL, (GLint*)&std::get<0>(state));
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint*)&std::get<1>(state));
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint*)&std::get<2>(state));
    return state;
  }
  void set(std::tuple<GLenum, GLenum, GLenum> state) override {
    glStencilOp(std::get<0>(state), std::get<1>(state), std::get<2>(state));
  }

  void set(GLenum sfail, GLenum dpfail, GLenum dppass) {
    set({sfail, dpfail, dppass});
  }
};

class ViewportState : public State<std::array<GLint, 4>> {
 public:
  std::array<GLint, 4> get() override {
    std::array<GLint, 4> state;
    glGetIntegerv(GL_VIEWPORT, state.data());
    return state;
  }

  void set(std::array<GLint, 4> state) override {
    glViewport(state[0], state[1], state[2], state[3]);
  }

  void set(GLint x, GLint y, GLsizei width, GLsizei height) {
    set({x, y, width, height});
  }
};

class ScissorBoxState : public State<std::array<GLint, 4>> {
 public:
  std::array<GLint, 4> get() override {
    std::array<GLint, 4> state;
    glGetIntegerv(GL_SCISSOR_BOX, state.data());
    return state;
  }

  void set(std::array<GLint, 4> state) override {
    glScissor(state[0], state[1], state[2], state[3]);
  }

  void set(GLint x, GLint y, GLsizei width, GLsizei height) {
    set({x, y, width, height});
  }
};

class Context {
 public:
  Context() {
    // depth_test_state_ = std::make_unique<DepthTestState>();
    // blend_state_ = std::make_unique<BlendState>();
    // cull_face_state_ = std::make_unique<CullFaceState>();
    // stencil_test_state_ = std::make_unique<StencilTestState>();
    // scissor_test_state_ = std::make_unique<ScissorTestState>();

    // depth_func_state_ = std::make_unique<DepthFuncState>();
    // depth_mask_state_ = std::make_unique<DepthMaskState>();
    // depth_range_state_ = std::make_unique<DepthRangeState>();

    // blend_func_state_ = std::make_unique<BlendFuncSeparateState>();
    // cull_face_mode_state_ = std::make_unique<CullFaceModeState>();
    // front_face_state_ = std::make_unique<FrontFaceState>();
    // stencil_func_state_ = std::make_unique<StencilFuncState>();
    // stencil_op_state_ = std::make_unique<StencilOpState>();
    // viewport_state_ = std::make_unique<ViewportState>();
    // scissor_box_state_ = std::make_unique<ScissorBoxState>();
  }

#pragma mark CAPABILITIES
  void pushDepthTest() { depth_test_state_.push(); }
  void popDepthTest() { depth_test_state_.pop(); }
  void setDepthTest(GLboolean val) { depth_test_state_.set(val); }
  GLboolean getDepthTest() { return depth_test_state_.get(); }

  void pushCullFace() { cull_face_state_.push(); }
  void popCullFace() { cull_face_state_.pop(); }
  void setCullFace(GLboolean val) { cull_face_state_.set(val); }
  void enableFaceCulling() { setCullFace(true); }
  void diableFaceCulling() { setCullFace(false); }
  GLboolean getCullFace() { return cull_face_state_.get(); }

  void pushBlend() { blend_state_.push(); }
  void popBlend() { blend_state_.pop(); }
  void setBlend(GLboolean val) { blend_state_.set(val); }
  void enableBlend() { setBlend(true); }
  void disableBlend() { setBlend(false); }
  GLboolean getBlend() { return blend_state_.get(); }

  void pushScissorTest() { scissor_test_state_.push(); }
  void popScissorTest() { scissor_test_state_.pop(); }
  void setScissorTest(GLboolean val) { scissor_test_state_.set(val); }
  GLboolean getScissorTest() { return scissor_test_state_.get(); }

  void pushStencilTest() { stencil_test_state_.push(); }
  void popStencilTest() { stencil_test_state_.pop(); }
  void setStencilTest(GLboolean val) { stencil_test_state_.set(val); }
  GLboolean getStencilTest() { return stencil_test_state_.get(); }

  //
#pragma mark DEPTH
  void pushDepthFunc() { depth_func_state_.push(); }
  void popDepthFunc() { depth_func_state_.pop(); }
  void setDepthFunc(GLenum mode) { depth_func_state_.set(mode); }
  GLenum getDepthFunc() { return depth_func_state_.get(); }

  void pushDepthMask() { depth_mask_state_.push(); }
  void popDepthMask() { depth_mask_state_.pop(); }
  void setDepthMask(GLboolean mode) { depth_mask_state_.set(mode); }
  GLboolean getDepthMask() { return depth_mask_state_.get(); }

  void pushDepthRange() { depth_range_state_.push(); }
  void popDepthRange() { depth_range_state_.pop(); }
  void setDepthRange(std::array<GLdouble, 2> range) {
    depth_range_state_.set(range);
  }
  void setDepthRange(GLdouble near, GLdouble far) {
    depth_range_state_.set({near, far});
  }
  std::array<GLdouble, 2> getDepthRange() { return depth_range_state_.get(); }

#pragma mark BLEND
  void pushBlendFunc() { blend_func_state_.push(); }
  void popBlendFunc() { blend_func_state_.pop(); }
  void setBlendFunc(std::array<GLenum, 4> func) { blend_func_state_.set(func); }
  void setBlendFunc(GLenum src, GLenum dst) {
    blend_func_state_.set({src, dst, src, dst});
  }
  void setBlendFuncSeparate(GLenum src, GLenum dst, GLenum src_alpha,
                            GLenum dst_alpha) {
    blend_func_state_.set({src, dst, src_alpha, dst_alpha});
  }
  std::array<GLenum, 4> getBlendFunc() { return blend_func_state_.get(); }
  // void setBlendAlpha() { blend_func_state_.setBlendAlpha(); }
  // void setBlendAdd() { blend_func_state_.setBlendAdd(); }
  // void setBlendMulti() { blend_func_state_.setBlendMulti(); }
  // void setBlendScreen() { blend_func_state_.setBlendScreen(); }
  // void setBlendReverse() { blend_func_state_.setBlendReverse(); }
  // void setBlendReverse2() { blend_func_state_.setBlendReverse2(); }
  // void setBlendSeparatedAlpha() {
  // blend_func_state_.setBlendSeparatedAlpha(); }

#pragma mark CULLING
  void pushCullFaceMode() { cull_face_mode_state_.push(); }
  void popCullFaceMode() { cull_face_mode_state_.pop(); }
  void setCullFaceMode(GLenum mode) { cull_face_mode_state_.set(mode); }
  GLenum getCullFaceMode() { return front_face_state_.get(); }

  void pushFrontFace() { front_face_state_.push(); }
  void popFrontFace() { front_face_state_.pop(); }
  void setFrontFace(GLenum mode) { front_face_state_.set(mode); }
  GLenum getFrontFace() { return front_face_state_.get(); }

#pragma mark STENCIL
  void pushStencilFunc() { stencil_func_state_.push(); }
  void popStencilFunc() { stencil_func_state_.pop(); }
  void setStencilFunc(std::tuple<GLenum, GLint, GLuint> func) {
    stencil_func_state_.set(func);
  }
  void setStencilFunc(GLenum func, GLint ref, GLuint mask) {
    setStencilFunc({func, ref, mask});
  }
  std::tuple<GLenum, GLint, GLuint> getStencilFunc() {
    return stencil_func_state_.get();
  }

  void pushStencilOp() { stencil_op_state_.push(); }
  void popStencilOp() { stencil_op_state_.pop(); }
  void setStencilOp(std::tuple<GLenum, GLenum, GLenum> op) {
    stencil_op_state_.set(op);
  }
  void setStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass) {
    setStencilOp({sfail, dpfail, dppass});
  }
  std::tuple<GLenum, GLenum, GLenum> getStencilOp() {
    return stencil_op_state_.get();
  }

#pragma mark VIEWPORT
  void pushViewport() { viewport_state_.push(); }
  void popViewport() { viewport_state_.pop(); }
  void setViewport(std::array<GLint, 4> viewport) {
    viewport_state_.set(viewport);
  }
  void setViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    viewport_state_.set({x, y, width, height});
  }
  std::array<GLint, 4> getViewport() { return viewport_state_.get(); }

#pragma mark SCISSOR
  void pushScissorBox() { scissor_box_state_.push(); }
  void popScissorBox() { scissor_box_state_.pop(); }
  void setScissorBox(std::array<GLint, 4> viewport) {
    scissor_box_state_.set(viewport);
  }
  void setScissorBox(GLint x, GLint y, GLsizei width, GLsizei height) {
    setScissorBox({x, y, width, height});
  }
  std::array<GLint, 4> getScissorBox() { return scissor_box_state_.get(); }

 private:
  DepthTestState depth_test_state_;
  BlendState blend_state_;
  CullFaceState cull_face_state_;
  StencilTestState stencil_test_state_;
  ScissorTestState scissor_test_state_;

  DepthFuncState depth_func_state_;
  DepthMaskState depth_mask_state_;
  DepthRangeState depth_range_state_;
  BlendFuncSeparateState blend_func_state_;
  CullFaceModeState cull_face_mode_state_;
  FrontFaceState front_face_state_;
  StencilFuncState stencil_func_state_;
  StencilOpState stencil_op_state_;
  ViewportState viewport_state_;
  ScissorBoxState scissor_box_state_;
};
}  // namespace gl
}  // namespace rs