#pragma once
#include "app/Window.h"
#include "system/Observable.h"

namespace limas {

class Clickable {
 public:
  Clickable() : b_hovered_(false), b_pressed_(false) {}
  virtual ~Clickable() { disableInput(); }

  void enableInput(Window::Ptr& win) {
    conn_pressed_ = win->setOnMousePressed(this, &Clickable::mousePressed);
    conn_moved_ = win->setOnMouseMoved(this, &Clickable::mouseMoved);
    conn_released_ = win->setOnMouseReleased(this, &Clickable::mouseReleased);
  }

  void disableInput() {
    conn_pressed_.disconnect();
    conn_moved_.disconnect();
    conn_released_.disconnect();
  }

  bool isPressed() const { return b_pressed_; }
  bool isHovered() const { return b_hovered_; }

 protected:
  virtual bool isHovered(const MouseEventArgs& e) const = 0;
  virtual void onClicked(const MouseEventArgs& e) = 0;
  virtual void onReleased(const MouseEventArgs& e) = 0;
  virtual void onHovered(const MouseEventArgs& e) = 0;
  virtual void onUnHovered(const MouseEventArgs& e) = 0;
  virtual void onPressed(const MouseEventArgs& e) = 0;
  virtual void onDragged(const MouseEventArgs& e) = 0;

  bool b_pressed_;
  bool b_hovered_;

 private:
  void mousePressed(const MouseEventArgs& e) {
    if (b_hovered_) {
      b_pressed_ = true;
      onPressed(e);
    }
  }

  void mouseMoved(const MouseEventArgs& e) {
    bool b_last_hovered_ = b_hovered_;
    b_hovered_ = isHovered(e);
    if (b_hovered_ && !b_last_hovered_) {
      onHovered(e);
    } else if (!b_hovered_ && b_last_hovered_) {
      onUnHovered(e);
    }

    if (b_pressed_) {
      onDragged(e);
    }
  }
  void mouseReleased(const MouseEventArgs& e) {
    if (b_hovered_) onClicked(e);
    onReleased(e);
    b_pressed_ = false;
  }

  Connection conn_pressed_;
  Connection conn_moved_;
  Connection conn_released_;
};

class ClickableCircle : public Clickable {
 public:
  ClickableCircle() : Clickable() { setup(glm::vec2(0), 0); }
  ClickableCircle(const glm::vec2& pos, float radius) : Clickable() {
    setup(pos, radius);
  }

  void setup(const glm::vec2& pos, float radius) {
    setPosition(pos);
    setRadius(radius);
  }

  void setPosition(const glm::vec2& pos) { pos_ = pos; }
  void setOffset(const glm::vec2& offset) { offset_ = offset; }

  void setRadius(float radius) { radius_ = radius; }
  const glm::vec2& getPosition() const { return pos_; }
  float getRadius() const { return radius_; }

 protected:
  glm::vec2 pos_;
  glm::vec2 offset_;
  float radius_;

  virtual bool isHovered(const MouseEventArgs& e) const {
    float x = e.x + offset_.x;
    float y = e.y + offset_.y;
    float dist =
        sqrt((pos_.x - x) * (pos_.x - x) + (pos_.y - y) * (pos_.y - y));
    return dist < radius_;
  }

  virtual void onClicked(const MouseEventArgs& e) {}
  virtual void onReleased(const MouseEventArgs& e) {}
  virtual void onHovered(const MouseEventArgs& e) {}
  virtual void onUnHovered(const MouseEventArgs& e) {}
  virtual void onPressed(const MouseEventArgs& e) {}
  virtual void onDragged(const MouseEventArgs& e) {}
};

class ClickableRectangle : public Clickable {
 public:
  ClickableRectangle() : Clickable() { setup(glm::vec2(0), 0, 0); }
  ClickableRectangle(const glm::vec2& tl, float width, float height)
      : Clickable() {
    setup(tl, width, height);
  }

  void setup(const glm::vec2& tl, float width, float height) {
    setPosition(tl);
    setWidth(width);
    setHeight(height);
  }

  void setPosition(const glm::vec2& tl) { tl_ = tl; }
  void setOffset(const glm::vec2& offset) { offset_ = offset; }
  void setWidth(float width) { width_ = width; }
  void setHeight(float height) { height_ = height; }

  const glm::vec2& getPosition() const { return tl_; }
  float getWidth() const { return width_; }
  float getHeight() const { return height_; }

 protected:
  glm::vec2 tl_;
  glm::vec2 offset_;
  float width_;
  float height_;

  virtual bool isHovered(const MouseEventArgs& e) const {
    float x = e.x + offset_.x;
    float y = e.y + offset_.y;
    if (x >= tl_.x && x <= (tl_.x + width_) && y >= tl_.y &&
        y <= (tl_.y + height_)) {
      return true;
    } else {
      return false;
    }
  }

  virtual void onClicked(const MouseEventArgs& e) {}
  virtual void onReleased(const MouseEventArgs& e) {}
  virtual void onHovered(const MouseEventArgs& e) {}
  virtual void onUnHovered(const MouseEventArgs& e) {}
  virtual void onPressed(const MouseEventArgs& e) {}
  virtual void onDragged(const MouseEventArgs& e) {}
};

}  // namespace limas