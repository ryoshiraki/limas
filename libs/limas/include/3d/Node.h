#pragma once
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/quaternion.hpp"

namespace limas {

class Node {
 public:
  Node() { reset(); }

  void reset() {
    setPosition({0.f, 0.f, 0.f});
    setOrientation({0.f, 0.f, 0.f});
    setScale({1.f, 1.f, 1.f});
  }

  void lookAt(const glm::vec3& center) {
    auto diff = (getPosition() - center);
    auto r = glm::length(diff);
    if (r > 0) {
      float lat = acos(diff.y / r) - glm::half_pi<float>();
      float lon = atan2(diff.x, diff.z);
      glm::quat q = glm::angleAxis(0.f, glm::vec3(0, 0, 1)) *
                    glm::angleAxis(lon, glm::vec3(0, 1, 0)) *
                    glm::angleAxis(lat, glm::vec3(1, 0, 0));
      setOrientation(q);
    }
  }

  void lookAt(const glm::vec3& center, const glm::vec3& up) {
    auto zaxis = glm::normalize(getPosition() - center);
    if (glm::length(zaxis) > 0) {
      auto xaxis = glm::normalize(glm::cross(up, zaxis));
      auto yaxis = glm::cross(zaxis, xaxis);
      glm::mat3 m;
      m[0] = xaxis;
      m[1] = yaxis;
      m[2] = zaxis;
      setOrientation(glm::toQuat(m));
    }
  }

  void setPosition(const glm::vec3& p) {
    this->position_ = p;
    update();
  }

  void setOrientation(const glm::quat& q) {
    this->orientation_ = q;
    update();
  }

  void setOrientation(const glm::vec3& euler) {
    glm::quat q(glm::radians(euler));
    setOrientation(q);
    update();
  }

  void setScale(const glm::vec3& s) {
    this->scale_ = s;
    update();
  }

  const void setTransformMatrix(const glm::mat4& m) {
    transform_ = m;

    glm::vec3 scale;
    glm::quat orientation;
    glm::vec3 position;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform_, scale, orientation, position, skew, perspective);

    this->position_ = position;
    this->orientation_ = orientation;
    this->scale_ = scale;
  }

  const glm::mat4& getTransformMatrix() const { return transform_; }

  glm::vec3 getPosition() const { return glm::vec3(getTransformMatrix()[3]); }

  glm::quat getOrientation() const { return orientation_; }

  glm::vec3 getScale() const { return scale_; }

  glm::vec3 getXAxis() const {
    return glm::vec3(getTransformMatrix()[0] / scale_.x);
  }

  glm::vec3 getYAxis() const {
    return glm::vec3(getTransformMatrix()[1] / scale_.y);
  }

  glm::vec3 getZAxis() const {
    return glm::vec3(getTransformMatrix()[2] / scale_.z);
  }

  glm::vec3 getSideDir() const { return getXAxis(); }

  glm::vec3 getLookAtDir() const { return -getZAxis(); }

  glm::vec3 getUpDir() const { return getYAxis(); }

  void move(const glm::vec3& offset) {
    position_ += offset;
    update();
  }

  void truck(float amt) { move(getXAxis() * amt); }

  void boom(float amt) { move(getYAxis() * amt); }

  void dolly(float amt) { move(getZAxis() * amt); }

  void rotate(const glm::quat& q) {
    orientation_ = q * orientation_;
    update();
  }

  void rotate(float deg, const glm::vec3& v) {
    rotate(glm::angleAxis(glm::radians(deg), v));
  }

  void rotateAround(const glm::quat& q, const glm::vec3& center) {
    setPosition(q * (getPosition() - center) + center);
  }

  void rotateAroundDeg(float deg, const glm::vec3& axis,
                       const glm::vec3& center) {
    rotateAround(glm::angleAxis(glm::radians(deg), axis), center);
  }

  void tilt(float deg) { rotate(deg, getXAxis()); }

  void pan(float deg) { rotate(deg, getYAxis()); }

  void roll(float deg) { rotate(deg, getZAxis()); }

 protected:
  glm::mat4 transform_;
  glm::vec3 position_;
  glm::quat orientation_;
  glm::vec3 scale_;

  void update() {
    transform_ = glm::translate(glm::mat4(1.0), position_);
    transform_ = transform_ * glm::toMat4(orientation_);
    transform_ = glm::scale(transform_, scale_);
  }
};

}  // namespace limas
