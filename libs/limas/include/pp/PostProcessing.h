#pragma once
#include "pp/BasePass.h"
#include "primitives/Rectangle.h"

namespace rs {

class PostProcessing {
 public:
  PostProcessing() {}

  void setup(size_t width, size_t height) {
    for (int i = 0; i < 2; i++) {
      fbos_[i] = gl::Fbo::create(width, height);
      fbos_[i]->attachColor(GL_RGBA8);
      fbos_[i]->attachDepthAsTexture();
      for (int j = 0; j < 2; j++) {
        fbos_[i]->getTexture(j)->setMagFilter(GL_LINEAR);
        fbos_[i]->getTexture(j)->setMinFilter(GL_LINEAR);
        fbos_[i]->getTexture(j)->setWrapS(GL_CLAMP_TO_EDGE);
        fbos_[i]->getTexture(j)->setWrapT(GL_CLAMP_TO_EDGE);
      }
      fbos_[i]->bind();
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT);
      fbos_[i]->unbind();
    }
  }

  template <class T>
  pp::BasePass::Ptr addPass(const std::string& name) {
    auto pass = pp::BasePass::create<T>(name, fbos_[0]->getWidth(),
                                        fbos_[0]->getHeight());
    auto it = passes_.find(name);
    if (it == passes_.end()) {
      passes_[name] = pass;
    } else {
      log::warn("PostProcessing") << name << " was already added" << log::end();
    }
    return pass;
  }

  pp::BasePass::Ptr getPass(const std::string& name) {
    auto it = passes_.find(name);
    if (it != passes_.end()) {
      return it->second;
    }
    log::warn("PostProcessing") << name << " is not added" << log::end();
    return nullptr;
  }

  void bind() { fbos_.getFront()->bind(); }
  void unbind() {
    fbos_.getFront()->unbind();
    fbos_.swap();
    for (auto it = passes_.begin(); it != passes_.end(); ++it) {
      it->second->apply(fbos_);
    }
  }

  json serialize() const {
    json j;
    for (auto it = passes_.begin(); it != passes_.end(); ++it) {
      j[it->first] = it->second->serialize();
    }
    return j;
  }

  void deserialize(const json& j) {
    for (auto it = passes_.begin(); it != passes_.end(); ++it) {
      if (j.contains(it->first))
        it->second->deserialize(j[it->first]);
      else
        log::warn("PostProcessing")
            << it->first + " is not contained in json" << log::end();
    }
  }

  gl::Texture2D::Ptr& getTexture() { return fbos_.getBack()->getTexture(0); }

 private:
  PingPong<gl::Fbo::Ptr> fbos_;
  std::map<std::string, pp::BasePass::Ptr> passes_;
};

}  // namespace rs