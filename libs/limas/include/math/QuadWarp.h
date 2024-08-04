#pragma once
#include "math/Homography.h"
#include "math/Math.h"
#include "utils/Clickable.h"

namespace limas {

class QuadWarp;
class Corner : public ClickableCircle {
  friend class QuadWarp;

 public:
  Corner() : ClickableCircle() {}
  Corner(const glm::vec2& pos, float radius) : ClickableCircle(pos, radius) {}

 protected:
  glm::vec2 offset_;
  Observable<const MouseEventArgs> update_;

  virtual void onClicked(const MouseEventArgs& e) {}
  virtual void onReleased(const MouseEventArgs& e) {}
  virtual void onHovered(const MouseEventArgs& e) {}
  virtual void onUnHovered(const MouseEventArgs& e) {}
  virtual void onPressed(const MouseEventArgs& e) {
    offset_ = glm::vec2{e.x, e.y} - getPosition();
  }
  virtual void onDragged(const MouseEventArgs& e) {
    auto pos = glm::vec2{e.x, e.y} - offset_;
    setPosition(pos);
    update_.notify(e);
  }
};

class QuadWarp {
 public:
  QuadWarp() {
    setInputArea(0, 0, 1, 1);
    setOutputArea(0, 0, 1, 1);
  }
  virtual ~QuadWarp() {
    disableInputAreaControllable();
    disableOutputAreaControllable();
  }

  void setInputArea(const glm::vec2& tl, const glm::vec2& tr,
                    const glm::vec2& br, const glm::vec2& bl) {
    src_corners_[0].setPosition(tl);
    src_corners_[1].setPosition(tr);
    src_corners_[2].setPosition(br);
    src_corners_[3].setPosition(bl);

    homography_ = getHomography();
  }

  void setInputArea(float x, float y, float width, float height) {
    setInputArea(glm::vec2{x, y}, glm::vec2{x + width, y},
                 glm::vec2{x + width, y + height}, glm::vec2{x, y + height});
  }

  void setOutputArea(const glm::vec2& tl, const glm::vec2& tr,
                     const glm::vec2& br, const glm::vec2& bl) {
    dst_corners_[0].setPosition(tl);
    dst_corners_[1].setPosition(tr);
    dst_corners_[2].setPosition(br);
    dst_corners_[3].setPosition(bl);

    homography_ = getHomography();
  }

  void setOutputArea(float x, float y, float width, float height) {
    setOutputArea(glm::vec2{x, y}, glm::vec2{x + width, y},
                  glm::vec2{x + width, y + height}, glm::vec2{x, y + height});
  }

  void enableInputAreaControllable(Window::Ptr& window) {
    for (int i = 0; i < 4; i++) {
      src_corners_[i].enableInput(window);
      src_conn_[i] =
          src_corners_[i].update_.addObserver(this, &QuadWarp::update);
    }
  }

  void enableOutputAreaControllable(Window::Ptr& window) {
    for (int i = 0; i < 4; i++) {
      dst_corners_[i].enableInput(window);
      dst_conn_[i] =
          dst_corners_[i].update_.addObserver(this, &QuadWarp::update);
    }
  }

  void disableInputAreaControllable() {
    for (int i = 0; i < 4; i++) {
      src_corners_[i].disableInput();
      src_conn_[i].disconnect();
    }
  }

  void disableOutputAreaControllable() {
    for (int i = 0; i < 4; i++) {
      dst_corners_[i].disableInput();
      dst_conn_[i].disconnect();
    }
  }

  void setCornerRadius(float radius) {
    for (int i = 0; i < 4; i++) {
      src_corners_[i].setRadius(radius);
      dst_corners_[i].setRadius(radius);
    }
  }

  glm::mat4 getHomography() const {
    glm::vec2 src_quad[4];
    for (int i = 0; i < 4; i++) {
      src_quad[i] = src_corners_[i].getPosition();
    }

    glm::vec2 dst_quad[4];
    for (int i = 0; i < 4; i++) {
      dst_quad[i] = dst_corners_[i].getPosition();
    }
    return math::Homography::getHomography(src_quad, dst_quad);
  }

  glm::vec2 getWarped(const glm::vec2& p) const {
    float x = p.x;
    // math::clamp(p.x, tl.x, tl.x + width);
    float y = p.y;
    // math::clamp(p.y, tl.y, tl.y + height);
    glm::vec4 src = glm::vec4(x, y, 0, 1);
    glm::vec4 dst = homography_ * src;
    return glm::vec2(dst.x / dst.w, dst.y / dst.w);
  }

  const Corner& getInputCorner(int index) const { return src_corners_[index]; }
  const Corner& getOutputCorner(int index) const { return dst_corners_[index]; }

 protected:
  glm::mat4 homography_;
  Corner dst_corners_[4];
  Corner src_corners_[4];
  Connection src_conn_[4];
  Connection dst_conn_[4];

  void update(const MouseEventArgs& e) { homography_ = getHomography(); }
};

}  // namespace limas
