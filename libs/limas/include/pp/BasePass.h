#pragma once
#include "gl/Fbo.h"
#include "gl/Shader.h"
#include "gl/VboMesh.h"
#include "primitives/Rectangle.h"
#include "type/ParameterGroup.h"
#include "type/PingPong.h"

namespace limas {
namespace pp {

class BasePass : public std::enable_shared_from_this<BasePass> {
 public:
  using Ptr = std::shared_ptr<BasePass>;

  template <typename T>
  static Ptr create(const std::string& name, size_t width, size_t height) {
    Ptr instance(new T(name, width, height));
    return instance;
  }

  virtual void apply(PingPong<gl::Fbo::Ptr>& fbos) = 0;

  template <typename T>
  Ptr setParam(const std::string& name, const T& value) {
    params_.setValue<T>(name, value);
    return shared_from_this();
  }

  template <typename T>
  Ptr setParam(const std::string& group, const std::string& name,
               const T& value) {
    params_.getChild(group).setValue<T>(name, value);
    return shared_from_this();
  }

  template <typename T>
  T getParam(const std::string& name) {
    return params_.getValue<T>(name);
  }

  template <typename T>
  T getParam(const std::string& group, const std::string& name) {
    return params_.getChild(group).getValue<T>(name);
  }

  json serialize() const { return params_.serialize(); }
  void deserialize(const json& j) { params_.deserialize(j); }

 protected:
  BasePass(const std::string& name) {
    plane_ = prim::Rectangle(-1, -1, 2, 2);
    params_.setName(name);
  }
  void fill() const { plane_.draw(GL_TRIANGLE_FAN); }

  ParameterGroup& getParams() { return params_; }

  template <typename T>
  Parameter<T>& addParam(const std::string& name, const T& value) {
    return params_.addParam<T>(name, value);
  }

  template <typename T>
  Parameter<T>& addParam(const std::string& group, const std::string& name,
                         const T& value) {
    if (!params_.hasChild(group)) params_.addChild(group);
    return params_.getChild(group).addParam<T>(group, value);
  }

 private:
  gl::VboMesh plane_;
  ParameterGroup params_;
};

}  // namespace pp
}  // namespace limas