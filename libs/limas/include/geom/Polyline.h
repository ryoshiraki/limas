#pragma once
#include "system/Logger.h"

namespace limas {
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

  float getArea() const {
    float area = 0.0f;
    for (std::size_t i = 1; i < vertices_.size() - 1; ++i) {
      auto a = vertices_[i] - vertices_[0];
      auto b = vertices_[i + 1] - vertices_[0];
      auto c = glm::cross(a, b);
      area += glm::length(c);
    }
    return area * 0.5f;
  }

  double getLength() const {
    double length = 0.0f;
    for (std::size_t i = 1; i < vertices_.size(); ++i) {
      length += distance(vertices_[i - 1], vertices_[i]);
    }
    return length;
  }

  V getPointAtLength(float dist) const {
    float interpolatedIndex = getIndexAtLength(dist);
    std::size_t index = static_cast<std::size_t>(std::floor(interpolatedIndex));
    float t = interpolatedIndex - index;

    if (index >= vertices_.size() - 1) return vertices_.back();
    return mix(vertices_[index], vertices_[index + 1], t);
  }

  float getIndexAtLength(float dist) const {
    if (dist <= 0) return 0;

    float length = 0.0f;
    for (std::size_t i = 1; i < vertices_.size(); ++i) {
      float seg_length = distance(vertices_[i - 1], vertices_[i]);
      if (length + seg_length >= dist) {
        float t = (dist - length) / seg_length;
        return i - 1 + t;
      }
      length += seg_length;
    }
    return vertices_.size() - 1;
  }

  V getPointAt(size_t index) const {
    if (index >= vertices_.size()) {
      logger::warn("Polyline")
          << "getPointAt(): Index out of bounds: " << index << logger::end();
      return vertices_.back();
    }
    return vertices_[index];
  }

  V getPointAtInterpolated(float t) const {
    if (vertices_.size() < 2) {
      logger::warn("Polyline")
          << "getPointAtInterpolated(): Not enough vertices to interpolate."
          << logger::end();
      return V(0);
    }

    t = std::clamp(t, 0.0f, static_cast<float>(vertices_.size() - 1));
    size_t index = static_cast<size_t>(t);
    float fraction = t - index;

    V p0 = getPointAt(index);
    V p1 = (index < vertices_.size() - 1) ? getPointAt(index + 1) : p0;

    return mix(p0, p1, fraction);
  }

  V getTangentAt(size_t index) const {
    if (index == 0) {
      return glm::normalize(vertices_[1] - vertices_[0]);
    }
    if (index >= vertices_.size() - 1) {
      return glm::normalize(vertices_.back() - vertices_[vertices_.size() - 2]);
    }

    return glm::normalize(vertices_[index + 1] - vertices_[index]);
  }

  V getTangentAtInterpolated(float t) const {
    if (vertices_.size() < 2) {
      logger::warn("Polyline") << "getTangentAtInterpolated(): Not enough "
                                  "vertices to compute tangent."
                               << logger::end();
      return V(0);
    }

    t = std::clamp(t, 0.0f, static_cast<float>(vertices_.size() - 1));
    size_t index = static_cast<size_t>(t);
    float fraction = t - index;

    V tangent1 = getTangentAt(index);
    V tangent2 =
        (index < vertices_.size() - 1) ? getTangentAt(index + 1) : tangent1;

    V interpolated_tangent = mix(tangent1, tangent2, fraction);
    return glm::normalize(interpolated_tangent);
  }

  float getCurvatureAt(float t) const {
    if (vertices_.size() < 3) {
      logger::warn("Polyline")
          << "getCurvatureAt(): Not enough vertices to compute curvature."
          << logger::end();
      return 0.0f;
    }

    t = std::clamp(t, 0.0f, static_cast<float>(vertices_.size() - 1));

    V tangent1 = getTangentAtInterpolated(t - 0.1f);
    V tangent2 = getTangentAtInterpolated(t + 0.1f);

    float dot_product = glm::dot(tangent1, tangent2);
    dot_product = std::clamp(dot_product, -1.0f, 1.0f);
    float angle = std::acos(dot_product);

    float segment_length = glm::length(getPointAtInterpolated(t + 0.1f) -
                                       getPointAtInterpolated(t - 0.1f));
    if (segment_length > 0) {
      return angle / segment_length;
    } else {
      return 0.0f;
    }
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
      logger::warn("Polyline")
          << "getResampledByCount(): requested " << count
          << " points, using minimum count of 2 " << logger::end();
      count = 2;
    }

    float space = total_length / (count - 1);
    return BasePolyline<V>::getResampledBySpacing(space);
  }

  BasePolyline<V> getSmoothedWithMovingAverage(int window_size) const {
    if (vertices_.empty()) return *this;

    BasePolyline<V> smoothed;
    int half_window = window_size / 2;

    for (size_t i = 0; i < vertices_.size(); ++i) {
      V sum(0);
      int count = 0;

      for (int j = -half_window; j <= half_window; ++j) {
        size_t idx = i + j;
        if (idx >= 0 && idx < vertices_.size()) {
          sum += vertices_[idx];
          ++count;
        }
      }

      smoothed.addVertex(sum / static_cast<float>(count));
    }

    return smoothed;
  }

  BasePolyline<V> getSmoothedWithGaussian(int window_size, float sigma) const {
    if (vertices_.empty()) return *this;

    BasePolyline<V> smoothed;
    std::vector<float> kernel = createGaussianKernel(window_size, sigma);
    int half_window = window_size / 2;

    for (size_t i = 0; i < vertices_.size(); ++i) {
      V sum(0);
      float weight_sum = 0.0f;

      for (int j = -half_window; j <= half_window; ++j) {
        size_t idx = i + j;
        if (idx >= 0 && idx < vertices_.size()) {
          sum += vertices_[idx] * kernel[j + half_window];
          weight_sum += kernel[j + half_window];
        }
      }

      smoothed.addVertex(sum / weight_sum);
    }

    return smoothed;
  }

  BasePolyline<V> getSmoothed(int iterations) {
    auto vertices = getVertices();

    for (int iter = 0; iter < iterations; ++iter) {
      std::vector<V> new_vertices;

      new_vertices.push_back(vertices[0]);

      for (size_t i = 0; i < vertices.size() - 1; ++i) {
        const V& p0 = vertices[i];
        const V& p1 = vertices[i + 1];

        V q = 0.75f * p0 + 0.25f * p1;
        V r = 0.25f * p0 + 0.75f * p1;

        new_vertices.push_back(q);
        new_vertices.push_back(r);
      }

      new_vertices.push_back(vertices.back());

      vertices = std::move(new_vertices);
    }

    BasePolyline<V> new_polyline;
    new_polyline.addVertices(vertices);
    return new_polyline;
  }

  const V& operator[](int i) const { return vertices_[i]; }
  V& operator[](int i) { return vertices_[i]; }

 protected:
  V cross(V& a, V& b) const {
    return Type(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
                a[0] * b[1] - a[1] * b[0]);
  }

  float length(V& a) const {
    float len2 = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
    return (len2 > 0.0f) ? std::sqrt(len2) : 0.0f;
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

  std::vector<float> createGaussianKernel(int window_size, float sigma) const {
    std::vector<float> kernel(window_size);
    int halfWindow = window_size / 2;
    float sum = 0.0f;

    for (int i = 0; i < window_size; ++i) {
      float x = float(i - halfWindow);
      kernel[i] = std::exp(-0.5f * (x * x) / (sigma * sigma));
      sum += kernel[i];
    }

    // 正規化
    for (float& k : kernel) {
      k /= sum;
    }

    return kernel;
  }

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
}  // namespace limas