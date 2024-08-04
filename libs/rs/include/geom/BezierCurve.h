#pragma once
#include "bezier.hpp"

namespace rs {
namespace geom {

using namespace std;

template <class T, int N>
class BezierCurve {
 public:
  bool generate(const vector<T>& _control_points) {
    if (_control_points.size() != N + 1) {
      cerr << "num of control points must be " << N + 1 << endl;
      return false;
    }
    vector<Bezier::Point> control_points;
    transform(cbegin(_control_points), cend(_control_points),
              back_inserter(control_points), [](T p) {
                return Bezier::Point{p.x, p.y};
              });

    bezier_ = make_shared<Bezier::Bezier<N>>(control_points);
    return true;
  }

  size_t size() const {
    if (bezier_ == nullptr) return 0;
    return bezier_->size();
  }

  vector<T> getPoints(int num_segments) const {
    vector<T> output;
    if (bezier_ == nullptr || num_segments < size()) return;
    for (int i = 0; i < num_segments; i++) {
      float t = (float)i / num_segments;
      output.emplace_back(getPointAt(t));
    }
    return output;
  }

  T getPointAt(float pct) const {
    if (bezier_ == nullptr) return T(0);
    auto p = bezier_->valueAt(pct);
    return {p.x, p.y};
  }

  T getTangentAt(float pct) const {
    if (bezier_ == nullptr) return T(0);
    auto tangent = bezier_->tangentAt(pct);
    return {tangent.x, tangent.y};
  }

  float getLength() const {
    if (bezier_ == nullptr) return 0;
    return bezier_->length();
  }

  shared_ptr<Bezier::Bezier<N>> bezier_;
};

template <class T>
class CubicBezier : public BezierCurve<T, 3> {};

template <class T>
class QuadraticBezier : public BezierCurve<T, 2> {};

}  // namespace geom
}  // namespace rs
