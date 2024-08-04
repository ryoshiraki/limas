#pragma once
#include "math/Math.h"

namespace limas {
class MatrixStack : public std::stack<glm::mat4> {
 public:
  MatrixStack() { push(glm::mat4(1.0f)); }

  glm::mat4& get() { return top(); }

  MatrixStack& push() {
    emplace(top());
    return *this;
  }

  MatrixStack& pop() {
    std::stack<glm::mat4>::pop();
    assert(!empty());
    return *this;
  }

  MatrixStack& push(const glm::mat4& mat) {
    std::stack<glm::mat4>::push(mat);
    return *this;
  }

  MatrixStack& identity() {
    top() = glm::mat4(1.0f);
    return *this;
  }

  MatrixStack& mult(const glm::mat4& mat) {
    top() *= mat;
    return *this;
  }

  MatrixStack& translate(const glm::vec3& translation) {
    return mult(glm::translate(glm::mat4(1), translation));
  }

  MatrixStack& rotate(float theta, const glm::vec3& axis) {
    return mult(glm::rotate(glm::mat4(1), theta, axis));
  }

  MatrixStack& rotateRadian(float theta, const glm::vec3& axis) {
    return rotate(theta, axis);
  }

  MatrixStack& rotateDegree(float angle, const glm::vec3& axis) {
    return rotate(glm::radians(angle), axis);
  }

  MatrixStack& rotate(const glm::mat3& rotation) {
    return mult(glm::mat4(rotation));
  }

  MatrixStack& rotate(const glm::quat& rotation) {
    return mult(glm::mat4_cast(rotation));
  }

  MatrixStack& scale(const glm::vec3& scale) {
    return mult(glm::scale(glm::mat4(1), scale));
  }

  MatrixStack& transform(const glm::mat4& m) { return mult(m); }
};
}  // namespace limas
