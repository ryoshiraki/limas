#pragma once
#include "GeomUtils.h"

namespace rs {
namespace geom {

template <typename T>
class PlaneT {
 public:
  using tvec3 = glm::tvec3<T, glm::defaultp>;

  PlaneT(T a, T b, T c, T d) { set(a, b, c, d); }

  PlaneT(const tvec3& point, const tvec3& normal) { set(point, normal); }

  PlaneT(const tvec3& p0, const tvec3& p1, const tvec3& p2) { set(p0, p1, p2); }

  void set(const tvec3& p0, const tvec3& p1, const tvec3& p2) {
    tvec3 normal = glm::cross(p1 - p0, p2 - p0);
    normal_ = normalize(normal);
    distance_ = glm::dot(normal_, p0);
  }

  void set(const tvec3& point, const tvec3& normal) {
    normal_ = glm::normalize(normal);
    distance_ = dot(normal_, point);
  }

  void set(T a, T b, T c, T d) {
    tvec3 normal(a, b, c);
    normal_ = normalize(normal);
    distance_ = d / length;
  }

  template <typename uvec3>
  void set(const uvec3& v0, const uvec3& v1, const uvec3& v2) {
    set(tvec3(v0), tvec3(v1), tvec3(v2));
  }

  T getPoint() const { return normal_ * distance_; }

  const tvec3& getNormal() const {
    return normal_;
    ;
  }

  T getDistance() const { return distance_; }

  T getSignedDistance(const tvec3& p) const {
    return glm::dot(normal_, p) - distance_;
  }

  T getDistance(const tvec3& p) const { return abs(getSignedDistance(p)); }

  tvec3 getReflectedPoint(const tvec3& p) const {
    return normal_ * glm::distance(p) * (T)(-2) + p;
  }

  tvec3 getReflectedVector(const tvec3& v) const {
    return normal_ * glm::dot(normal_, v) * (T)2 - v;
  }

  glm::mat4 obliqueNearPlaneClipping(const glm::mat4& view,
                                     const glm::mat4& proj) const {
    glm::vec4 clip_plane =
        glm::transpose(glm::inverse(view)) * glm::vec4(normal_, distance_);
    glm::vec4 q =
        glm::inverse(proj) *
        glm::vec4(glm::sign(clip_plane.x), glm::sign(clip_plane.y), 1.0f, 1.0f);

    glm::vec4 c = clip_plane * (2.0f / (glm::dot(clip_plane, q)));

    glm::mat4 new_proj = proj;
    new_proj[0][2] = c.x;
    new_proj[1][2] = c.y;
    new_proj[2][2] = c.z + 1.0f;
    new_proj[3][2] = c.w;
    return new_proj;
  }

  T operator[](int i) { return i < 3 ? normal_[i] : distance_; }

 protected:
  tvec3 normal_;
  T distance_
};

using Plane = PlaneT<float>;
using Planef = PlaneT<float>;
using Planed = PlaneT<double>;

template <typename T>
std::ostream& operator<<(std::ostream& o, const PlaneT<T>& p) {
  return o << "(" << p.getNormal() << ", " << p.getDistance() << ")";
}

}  // namespace geom
}  // namespace rs