#pragma once
#include "app/Draw.h"
#include "gl/GL.h"
#include "graphics/Color.h"
#include "math/Math.h"
#include "utils/Clickable.h"

namespace limas {
class ColorHandle : public ClickableCircle {
 public:
  using Ptr = std::shared_ptr<ColorHandle>;

  ColorHandle() : ClickableCircle() {}
  ColorHandle(const FloatColor& col, const glm::vec2& pos, float radius)
      : ClickableCircle(pos, radius), col_(col), offset_(0) {}

  FloatColor& getColor() { return col_; }
  Observable<const MouseEventArgs>& getObservable() { return update_; }

 protected:
  FloatColor col_;
  float offset_;
  Observable<const MouseEventArgs> update_;

  virtual void onClicked(const MouseEventArgs& e) {}
  virtual void onReleased(const MouseEventArgs& e) {}
  virtual void onHovered(const MouseEventArgs& e) {}
  virtual void onUnHovered(const MouseEventArgs& e) {}
  virtual void onPressed(const MouseEventArgs& e) {
    offset_ = e.x - getPosition().x;
  }
  virtual void onDragged(const MouseEventArgs& e) {
    float x = e.x - offset_;
    float y = getPosition().y;
    setPosition(glm::vec2(x, y));
    update_.notify(e);
  }
};

class Swatch {
  constexpr static int HANDLE_RADIUS = 10.0f;

 public:
  Swatch() {}
  ~Swatch() {}

  void setup(Window::Ptr& win, size_t width, size_t height) {
    win_ = win;
    fbo_.allocate(width, height);
    fbo_.attachColor(GL_RGBA8);
    fbo_.getTexture(0).setMinFilter(GL_LINEAR);
    fbo_.getTexture(0).setMagFilter(GL_LINEAR);

    pbo_.allocate(fbo_.getWidth(), fbo_.getHeight(),
                  fbo_.getTextures()[0].getInternalFormat());
    pixels_.allocate(width, height, 4);
    shader_.load(fs::getCommonResourcesPath("shaders/swatch.vert"),
                 fs::getCommonResourcesPath("shaders/swatch.frag"));
  }

  void add(float pos, const FloatColor& col) {
    auto h = std::make_shared<ColorHandle>(
        col, glm::vec2(pos * fbo_.getWidth(), HANDLE_RADIUS + fbo_.getHeight()),
        HANDLE_RADIUS);
    handles_.push_back(h);
    h->enableInput(win_);
    h->getObservable().addObserver(this, &Swatch::_update);
  }

  void update() {
    if (handles_.empty()) return;

    std::sort(std::begin(handles_), std::end(handles_),
              [](ColorHandle::Ptr& a, ColorHandle::Ptr& b) {
                return a->getPosition().x < b->getPosition().x;
              });

    mesh.clear();

    mesh.addVertex(glm::vec3(0, 0, 0));
    mesh.addVertex(glm::vec3(0, fbo_.getHeight(), 0));

    auto front = handles_[0]->getColor().toGlm();
    mesh.addColor(glm::vec4(front));
    mesh.addColor(glm::vec4(front));

    for (int i = 0; i < handles_.size(); i++) {
      auto& c = handles_[i];
      float pos = c->getPosition().x;
      auto col = c->getColor().toGlm();

      mesh.addVertex(glm::vec3(pos, 0, 0));
      mesh.addVertex(glm::vec3(pos, fbo_.getHeight(), 0));

      mesh.addColor(glm::vec4(col));
      mesh.addColor(glm::vec4(col));

      mesh.addIndex(i * 2 + 0);
      mesh.addIndex(i * 2 + 1);
      mesh.addIndex(i * 2 + 2);

      mesh.addIndex(i * 2 + 2);
      mesh.addIndex(i * 2 + 3);
      mesh.addIndex(i * 2 + 1);
    }

    mesh.addVertex(glm::vec3(fbo_.getWidth(), 0, 0));
    mesh.addVertex(glm::vec3(fbo_.getWidth(), fbo_.getHeight(), 0));

    auto back = handles_.back()->getColor().toGlm();
    mesh.addColor(glm::vec4(back));
    mesh.addColor(glm::vec4(back));

    mesh.addIndex(handles_.size() * 2 + 0);
    mesh.addIndex(handles_.size() * 2 + 1);
    mesh.addIndex(handles_.size() * 2 + 2);

    mesh.addIndex(handles_.size() * 2 + 2);
    mesh.addIndex(handles_.size() * 2 + 3);
    mesh.addIndex(handles_.size() * 2 + 1);

    mesh.update();

    auto mvp = math::getOrthoProjection(fbo_.getWidth(), fbo_.getHeight());
    fbo_.bind();
    shader_.bind();
    shader_.setUniformMatrix4f("u_mvp", mvp);
    mesh.draw(GL_TRIANGLES);
    shader_.unbind();
    fbo_.unbind();
  }

  void drawTexture() const { gl::drawTexture(fbo_.getTexture(0), 0, 0); }
  void drawHandle() const {
    for (auto& h : handles_) {
      gl::drawCircle(h->getPosition().x, h->getPosition().y, h->getRadius());
    }
  }

  Pixels2D getPixels() {
    if (!pbo_.readTo(&pixels_.getData(), fbo_.getID(), 0)) {
      log::error("Swatch") << "failed to get pixels" << log::end();
    }
    return pixels_;
  }

 private:
  void _update(const MouseEventArgs& e) { update(); }

  Window::Ptr win_;
  gl::Fbo fbo_;
  gl::Shader shader_;
  gl::PboPacker pbo_;
  gl::VboMesh mesh;
  Pixels2D pixels_;
  std::vector<ColorHandle::Ptr> handles_;
};
}  // namespace limas