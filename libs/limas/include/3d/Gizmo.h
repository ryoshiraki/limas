#pragma once
#include "IGizmo.h"
#include "app/AppUtils.h"
#include "app/Window.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "system/Observable.h"

namespace limas {

enum GizmoType { GIZMO_MOVE = 0, GIZMO_SCALE, GIZMO_ROTATE };

class Gizmo {
 public:
  Gizmo() : window_(nullptr), b_enable_(false), b_interacting_(false) {
    gizmo_move_ = CreateMoveGizmo();
    gizmo_rotate_ = CreateRotateGizmo();
    gizmo_scale_ = CreateScaleGizmo();

    setType(GIZMO_MOVE);
    setDisplayScale(1.0);
    setMatrix(glm::mat4(1.0));
  }

  ~Gizmo() {
    disableInput();

    if (gizmo_move_ != nullptr) delete gizmo_move_;
    if (gizmo_rotate_ != nullptr) delete gizmo_rotate_;
    if (gizmo_scale_ != nullptr) delete gizmo_scale_;
  }

  void enableInput(Window::Ptr& window) {
    b_enable_ = true;

    window_ = window;
    pressed_ = window_->setOnMousePressed(this, &Gizmo::mousePressed);
    moved_ = window_->setOnMouseMoved(this, &Gizmo::mouseMoved);
    released_ = window_->setOnMouseReleased(this, &Gizmo::mouseReleased);
    resized_ = window_->setOnWindowResized(this, &Gizmo::windowResized);

    gizmo_rotate_->SetScreenDimension(window_->getWidth(),
                                      window_->getHeight());
    gizmo_move_->SetScreenDimension(window_->getWidth(), window_->getHeight());
    gizmo_scale_->SetScreenDimension(window_->getWidth(), window_->getHeight());
  }

  void disableInput() {
    b_enable_ = false;
    pressed_.disconnect();
    moved_.disconnect();
    released_.disconnect();
  }

  void setMatrix(const glm::mat4& obj_mat) {
    obj_mat_ = obj_mat;
    auto ptr = glm::value_ptr(obj_mat_);
    gizmo_rotate_->SetEditMatrix(ptr);
    gizmo_move_->SetEditMatrix(ptr);
    gizmo_scale_->SetEditMatrix(ptr);
  }

  glm::mat4 getMatrix() const { return obj_mat_; }

  glm::vec3 getTranslation() const { return glm::vec3(obj_mat_[3]); }

  glm::quat getRotation() const { return glm::quat(obj_mat_); }

  glm::vec3 getScale() const {
    glm::vec3 scale;
    glm::quat orientation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(obj_mat_, scale, orientation, translation, skew,
                   perspective);
    return scale;
  }

  void setTranslationAxisMask(unsigned int mask) {
    gizmo_move_->SetAxisMask(mask);
  }

  void setRotationAxisMask(unsigned int mask) {
    gizmo_rotate_->SetAxisMask(mask);
  }

  void setScaleAxisMask(unsigned int mask) { gizmo_scale_->SetAxisMask(mask); }

  void setType(GizmoType type) {
    if (type == GIZMO_ROTATE) {
      gizmo_ = gizmo_rotate_;
      gizmo_->SetLocation(IGizmo::LOCATE_LOCAL);
    } else if (type == GIZMO_MOVE) {
      gizmo_ = gizmo_move_;
      gizmo_->SetLocation(IGizmo::LOCATE_WORLD);
    } else if (type == GIZMO_SCALE) {
      gizmo_ = gizmo_scale_;
      gizmo_->SetLocation(IGizmo::LOCATE_WORLD);
    }
  }

  void setDisplayScale(float scale) {
    gizmo_rotate_->SetDisplayScale(scale);
    gizmo_move_->SetDisplayScale(scale);
    gizmo_scale_->SetDisplayScale(scale);
  }

  bool isEnable() const { return b_enable_; }

  bool isInteracting() const { return b_interacting_; }

  void draw(const glm::mat4& view, const glm::mat4& proj) {
    if (!b_enable_) return;

    const float* view_ptr = (const float*)glm::value_ptr(view);
    const float* proj_ptr = (const float*)glm::value_ptr(proj);

    gizmo_rotate_->SetCameraMatrix(view_ptr, proj_ptr);
    gizmo_move_->SetCameraMatrix(view_ptr, proj_ptr);
    gizmo_scale_->SetCameraMatrix(view_ptr, proj_ptr);

    gizmo_->Draw();
  }

  template <class ObserverClass>
  void setOnInteracting(ObserverClass* observer,
                        void (ObserverClass::*callback)(const bool&)) {
    conn_ = interacting_.addObserver(observer, callback);
  }

  void setOnInteracting(const std::function<void(const bool& b)>& callback) {
    conn_ = interacting_.addObserver(callback);
  }

  //  example
  //  gizmo.setOnInteracting([=](const bool& b_interacting) {
  //    b_interacting ?
  //    cam.disableInput() :
  //    cam.enableInput(getWindows()[0]);
  //  });

 private:
  void mouseMoved(const MouseEventArgs& e) { gizmo_->OnMouseMove(e.x, e.y); }

  void mousePressed(const MouseEventArgs& e) {
    if (gizmo_->OnMouseDown(e.x, e.y) && !b_interacting_) {
      b_interacting_ = true;
      interacting_.notify(b_interacting_);
    }
  }

  void mouseReleased(const MouseEventArgs& e) {
    gizmo_->OnMouseUp(e.x, e.y);
    if (b_interacting_) {
      b_interacting_ = false;
      interacting_.notify(b_interacting_);
    }
  }

  void windowResized(const ResizeEventArgs& e) {
    gizmo_rotate_->SetScreenDimension(window_->getWidth(),
                                      window_->getHeight());
    gizmo_move_->SetScreenDimension(window_->getWidth(), window_->getHeight());
    gizmo_scale_->SetScreenDimension(window_->getWidth(), window_->getHeight());
  }

  glm::mat4 obj_mat_;
  IGizmo* gizmo_ = nullptr;
  IGizmo* gizmo_move_ = nullptr;
  IGizmo* gizmo_rotate_ = nullptr;
  IGizmo* gizmo_scale_ = nullptr;

  bool b_enable_;
  bool b_interacting_;

  Window::Ptr window_;
  Connection pressed_, moved_, released_, resized_;
  Observable<const bool> interacting_;
  Connection conn_;
};

}  // namespace limas
