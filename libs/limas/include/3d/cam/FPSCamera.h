#pragma once
#include "3d/cam/Camera.h"
#include "app/Window.h"
#include "system/Observable.h"

namespace limas {

class FPSCamera : public Camera {
 public:
  FPSCamera()
      : Camera(),
        b_pressed_(false),
        pivot_dist_(0),
        control_params_({1.0f, 1.0f, 1.2f}) {
    setPosition({0, 0, pivot_dist_});
    lookAt({0, 0, 0}, {0, 1, 0});
  }

  void setDistanceToPivot(float dist) { pivot_dist_ = dist; }
  float getDistanceToPivot() const { return pivot_dist_; }

  void enableKeyInput(Window::Ptr& win) {
    key_pressed_ = win->setOnKeyPressed(this, &FPSCamera::keyPressed);
    key_released_ = win->setOnKeyReleased(this, &FPSCamera::keyReleased);
  }

  void enableMouseInput(Window::Ptr& win) {
    mouse_pressed_ = win->setOnMousePressed(this, &FPSCamera::mousePressed);
    mouse_moved_ = win->setOnMouseMoved(this, &FPSCamera::mouseMoved);
    mouse_released_ = win->setOnMouseReleased(this, &FPSCamera::mouseReleased);
    mouse_scrolled_ = win->setOnMouseScrolled(this, &FPSCamera::mouseScrolled);
  }

  void disableKeyInput() {
    key_pressed_.disconnect();
    key_released_.disconnect();
  }

  void disableMouseInput() {
    mouse_pressed_.disconnect();
    mouse_moved_.disconnect();
    mouse_released_.disconnect();
    mouse_scrolled_.disconnect();
  }

  void keyPressed(const KeyEventArgs& e) {
    float move_amt = 10.0 * control_params_.translate_sensitivity;
    switch (e.key) {
      case 'w':
        dolly(-move_amt);
        break;
      case 'a':
        truck(-move_amt);
        break;
      case 's':
        dolly(move_amt);
        break;
      case 'd':
        truck(move_amt);
        break;
      case 'q':
        boom(move_amt);
        break;
      case 'e':
        boom(-move_amt);
        break;
      default:
        break;
    }
  }

  void keyReleased(const KeyEventArgs& e) {}

  void mousePressed(const MouseEventArgs& e) {
    if (e.button == 0) {
      b_pressed_ = true;
      last_mouse_ = glm::vec2(e.x, e.y);
      last_axis_[0] = getSideDir();
      last_axis_[1] = getUpDir();
      last_axis_[2] = getLookAtDir();
      last_pos_ = getPosition();
      last_ori_ = getOrientation();
    }
  }

  void mouseMoved(const MouseEventArgs& e) {
    glm::vec2 mouse(e.x, e.y);
    if (b_pressed_) {
      auto rel = (mouse - last_mouse_) /
                 -(250.0f / control_params_.rotate_sensitivity);
      float dx = rel.x;
      float dy = -rel.y;

      glm::vec3 W = glm::normalize(last_axis_[2]);
      glm::vec3 U = glm::normalize(glm::cross(last_axis_[1], W));

      if ((last_ori_ * last_axis_[1]).y < 0.0f) {
        dx = -dx;
        dy = -dy;
      }

      glm::vec3 rotate_axis =
          glm::angleAxis(dy, U) * (-last_axis_[2] * getDistanceToPivot());
      rotate_axis = glm::angleAxis(dx, last_axis_[1]) * rotate_axis;

      setPosition(last_pos_ + last_axis_[2] * getDistanceToPivot() +
                  rotate_axis);
      setOrientation(glm::angleAxis(dx, last_axis_[1]) * glm::angleAxis(dy, U) *
                     last_ori_);
    }
  }

  void mouseReleased(const MouseEventArgs& e) { b_pressed_ = false; }

  void mouseScrolled(const ScrollEventArgs& e) {
    float multiplier = powf(control_params_.zoom_sensitivity + 0.1f, e.y);
    glm::vec3 new_pos = getPosition() + getLookAtDir() * (getDistanceToPivot() *
                                                          (1 - multiplier));
    setPosition(new_pos);
    setDistanceToPivot(std::max<float>(getDistanceToPivot() * multiplier, 1.0));
  }

  void setTranslateSensitivity(float v) {
    control_params_.translate_sensitivity = v;
  }
  float getTranslateSensitivity() const {
    return control_params_.translate_sensitivity;
  }

  void setRotateSensitivity(float v) { control_params_.rotate_sensitivity = v; }
  float getRotateSensitivity() const {
    return control_params_.rotate_sensitivity;
  }

  void seZoomSensitivity(float v) { control_params_.zoom_sensitivity = v; }
  float getZoomSensitivity() const { return control_params_.zoom_sensitivity; }

 protected:
  bool b_pressed_;
  glm::vec2 last_mouse_;
  glm::vec3 last_pos_;
  glm::vec3 last_axis_[3];
  glm::quat last_ori_;
  float pivot_dist_;

  struct ControlParams {
    float translate_sensitivity = 1.0;
    float rotate_sensitivity = 1.0;
    float zoom_sensitivity = 1.0;
  };
  ControlParams control_params_;

  Connection key_pressed_;
  Connection key_released_;
  Connection mouse_pressed_;
  Connection mouse_moved_;
  Connection mouse_released_;
  Connection mouse_scrolled_;
};

}  // namespace limas
