#pragma once
#include "geom/Mesh.h"

namespace rs {
namespace geom {

template <class T, int dim = 3>
class AABB {
  T abs(const T& a) {
    T res;
    for (int i = 0; i < dim; i++) {
      res[i] = std::abs(a[i]);
    }
    return res;
  }

  float dot(const T& a, const T& b) {
    float d = 0;
    for (int i = 0; i < dim; i++) {
      d += a[i] * b[i];
    }
    return d;
  }

  T maximum(const T& a, float v) {
    T res;
    for (int i = 0; i < dim; i++) {
      res[i] = std::max(a[i], v);
    }
    return res;
  }

 public:
  AABB() { reset(); }
  AABB(const std::vector<T>& pts) { add(pts); }

  float getWidth() const { return std::abs(max_[0] - min_[0]); }
  float getHeight() const { return std::abs(max_[1] - min_[1]); }
  float getDepth() const { return std::abs(max_[2] - min_[2]); }
  T getSize() const { return max_ - min_; }
  float getVolume() const {
    return getWidth() * getHeight() * (dim > 2 ? getDepth() : 1);
  }
  T getMin() const { return min_; }
  T getMax() const { return max_; }
  T getCenter() const { return (min_ + max_) * 0.5f; }

  void reset() {
    for (int i = 0; i < dim; i++) {
      min_[i] = INT_MAX;
      max_[i] = -INT_MAX;
    }
  }

  void add(const std::vector<T>& pts) {
    for (auto& p : pts) add(p);
  }

  void add(const T& p) {
    for (int i = 0; i < dim; i++) {
      if (p[i] < min_[i]) min_[i] = p[i];
      if (p[i] > max_[i]) max_[i] = p[i];
    }
  }

  void add(const AABB<T, dim>& box) {
    for (int i = 0; i < dim; i++) {
      if (box.min_[i] < min_[i]) min_[i] = box.min_[i];
      if (box.max_[i] > max_[i]) max_[i] = box.max_[i];
    }
  }

  bool empty() const {
    for (int i = 0; i < dim; i++) {
      if (min_[i] <= max_[i]) return false;
    }
    return true;
  }

  bool isInside(const T& p) const {
    for (int i = 0; i < dim; i++) {
      if (p[i] < min_[i] || p[i] > max_[i]) return false;
    }
    return true;
  }

  bool isOverlapped(const AABB<T, dim>& other,
                    AABB<T, dim>* overlap = nullptr) {
    auto min = other.getMin();
    auto max = other.getMax();
    bool r = true;

    for (int i = 0; i < dim; i++) {
      r = r && ((min_[i] <= max[i]) && (max_[i] >= min[i]));
    }

    if (r && overlap != nullptr) {
      for (int i = 0; i < dim; i++) {
        overlap->min_[i] = std::max(min_[i], min[i]);
        overlap->max_[i] = std::min(max_[i], max[i]);
      }
    }

    return r;
  }

  AABB<T, dim> getOverlapped(const AABB<T, dim>& other) {
    AABB<T, dim> output;
    isOverlapped(other, &output);
    return output;
  }

  bool isOverlappedWithSphere(const T& center, float radius) {
    T c = getCenter();
    T h = getSize();
    T v = abs(center - c);
    T u = maximum(v - h, 0);
    return dot(u, u) < (radius * radius);
  }

  void translate(const T& amt) {
    min_ += amt;
    max_ += amt;
  }

  geom::Mesh toMesh() const {
    float min_depth = dim > 2 ? min_[2] : 0;
    float max_depth = dim > 2 ? max_[2] : 0;

    std::vector<int> indices;
    if (dim == 3) {
      indices = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
    } else {
      indices = {0, 1, 1, 2, 2, 3, 3, 0};
    }

    std::vector<glm::vec3> vertices(dim == 3 ? 8 : 4);
    vertices[0] = {min_[0], min_[1], min_depth};
    vertices[1] = {max_[0], min_[1], min_depth};
    vertices[2] = {max_[0], max_[1], min_depth};
    vertices[3] = {min_[0], max_[1], min_depth};
    if (dim == 3) {
      vertices[4] = {min_[0], min_[1], max_depth};
      vertices[5] = {max_[0], min_[1], max_depth};
      vertices[6] = {max_[0], max_[1], max_depth};
      vertices[7] = {min_[0], max_[1], max_depth};
    }

    geom::Mesh mesh;
    mesh.setVertices(vertices);
    mesh.setIndices(indices);
    return mesh;
  }

  T min_;
  T max_;
};

template <class T, int dim = 3>
std::ostream& operator<<(std::ostream& os, const AABB<T, dim>& aabb) {
  auto min = aabb.getMin();
  auto max = aabb.getMax();
  auto size = aabb.getSize();

  os << "min = (";
  for (int i = 0; i < dim - 1; i++) {
    os << min[i] << ", ";
  }
  os << min[dim - 1] << ")\n";

  os << "max = (";
  for (int i = 0; i < dim - 1; i++) {
    os << max[i] << ", ";
  }
  os << max[dim - 1] << ")\n";

  os << "size = (";
  for (int i = 0; i < dim - 1; i++) {
    os << size[i] << ", ";
  }
  os << size[dim - 1] << ")\n";

  return os;
}

using AABB2D = AABB<glm::vec2, 2>;
using AABB3D = AABB<glm::vec3, 3>;

}  // namespace geom
}  // namespace rs
