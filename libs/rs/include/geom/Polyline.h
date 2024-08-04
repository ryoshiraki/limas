#pragma once
#include "system/Logger.h"

namespace rs {
namespace geom {

template <class V>
class BasePolyline {
 public:
  BasePolyline() {}

  void clear() { vertices_.clear(); }

  void addVertex(const V& v) { vertices_.push_back(v); }
  void addVertices(const std::vector<V>& v) {
    copy(v.begin(), v.end(), std::back_inserter(vertices_));
  }
  void setVertices(const std::vector<V>& v) { vertices_ = v; }

  std::vector<V>& getVertices() { return vertices_; }
  const std::vector<V>& getVertices() const { return vertices_; }

  size_t getNumVertices() const { return vertices_.size(); }

  float getLength() const {
    float length = 0.0f;
    for (std::size_t i = 1; i < vertices_.size(); ++i) {
      length += distance(vertices_[i - 1], vertices_[i]);
    }
    return length;
  }

  V getPointAtLength(float dist) const {
    float length = 0.0f;
    for (std::size_t i = 1; i < vertices_.size(); ++i) {
      float seg_length = distance(vertices_[i - 1], vertices_[i]);
      if (length + seg_length >= dist) {
        float t = (dist - length) / seg_length;
        return mix(vertices_[i - 1], vertices_[i], t);
      }
      length += seg_length;
    }
    return vertices_.back();
  }

  BasePolyline<V> getResampledBySpacing(float space) const {
    if (space == 0 || getNumVertices() == 0) return *this;
    BasePolyline<V> poly;
    float total_length = getLength();
    float length = 0;
    for (float length = 0; length <= total_length; length += space) {
      auto v = getPointAtLength(length);
      poly.addVertex(v);
    }
    return poly;
  }

  BasePolyline<V> getResampledByCount(int count) const {
    float total_length = getLength();
    if (count < 2) {
      log::warn("Polyline")
          << "getResampledByCount(): requested " << count
          << " points, using minimum count of 2 " << log::end();
      count = 2;
    }

    float space = total_length / (count - 1);
    return BasePolyline<V>::getResampledBySpacing(space);
  }

  const V& operator[](int i) const { return vertices_[i]; }
  V& operator[](int i) { return vertices_[i]; }

 protected:
  V cross(V& a, V& b) const {
    return Type(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
                a[0] * b[1] - a[1] * b[0]);
  }

  float length(V& a) const {
    return std::sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  }

  float distance(const V& a, const V& b) const {
    V c = b - a;
    return length(c);
  }

  V normalize(V& a) {
    float len = length(a);
    V b;
    b[0] = a[0] / len;
    b[1] = a[1] / len;
    b[2] = a[2] / len;
    return b;
  }

  V mix(const V& a, const V& b, float t) const { return a + (b - a) * t; }

  std::vector<V> vertices_;
};

template <class V>
inline std::ostream& operator<<(std::ostream& os, const BasePolyline<V>& poly) {
  os << "V:";
  os << poly.getNumVertices();
  return os;
}

using Polyline = BasePolyline<glm::vec3>;

}  // namespace geom
}  // namespace rs