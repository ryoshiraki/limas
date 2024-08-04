#pragma once
#include "splines/cubic_hermite_spline.h"
#include "splines/generic_b_spline.h"
#include "splines/natural_spline.h"
#include "splines/quintic_hermite_spline.h"
#include "splines/uniform_cr_spline.h"
#include "splines/uniform_cubic_bspline.h"

namespace limas {
namespace geom {

enum SplineType {
  CR_SPLINE = 0,
  CUBIC_B_SPLINE,
  NATURAL_SPLINE,
  CUBIC_HERMITATE_SPLINE,
  GENERIC_B_SPLINE
};

template <class T>
class BaseSplineCurve {
  using Points = std::vector<T>;

 public:
  BaseSplineCurve() : spline_(nullptr) {}

  bool generateCubicHermiteSpline(const Points& control_points, float alpha) {
    return generate(CUBIC_HERMITATE_SPLINE, control_points, alpha);
  }
  bool generateCRSpline(const Points& control_points) {
    return generate(CR_SPLINE, control_points);
  }
  bool generateCentripetalCRSpline(const Points& control_points) {
    return generateCubicHermiteSpline(control_points, 0.5);
  }
  bool generateChordalCRSpline(const Points& control_points) {
    return generateCubicHermiteSpline(control_points, 1.0);
  }
  bool generateGenericBSpline(const Points& control_points, float degree) {
    return generate(GENERIC_B_SPLINE, control_points, degree);
  }
  bool generateQuadraticBSpline(const Points& control_points, float degree) {
    return generateGenericBSpline(control_points, 2);
  }
  bool generateUniformCubicBSpline(const Points& control_points) {
    return generate(CUBIC_B_SPLINE, control_points);
  }
  bool generateQuarticBSpline(const Points& control_points, float degree) {
    return generateGenericBSpline(control_points, 4);
  }
  bool generateNaturalSpline(const Points& control_points) {
    return generate(NATURAL_SPLINE, control_points);
  }

  bool generate(SplineType type, const Points& control_points,
                float param = 0) {
    switch (type) {
      case CR_SPLINE:
        spline_ = std::make_shared<UniformCRSpline<T, float>>(control_points);
        break;
      case CUBIC_B_SPLINE:
        spline_ =
            std::make_shared<UniformCubicBSpline<T, float>>(control_points);
        break;
      case NATURAL_SPLINE:
        spline_ = std::make_shared<NaturalSpline<T, float>>(control_points);
        break;
      case CUBIC_HERMITATE_SPLINE:
        spline_ = std::make_shared<CubicHermiteSpline<T, float>>(control_points,
                                                                 param);
        break;
      case GENERIC_B_SPLINE:
        spline_ =
            std::make_shared<GenericBSpline<T, float>>(control_points, param);
        break;
      default:
        return false;
    }

    return true;
  }

  size_t size() const {
    if (spline_ == nullptr) return 0;
    return spline_->getMaxT();
  }

  Points getPoints(int num_segments) const {
    Points output;
    if (spline_ == nullptr || num_segments < size()) return output;
    for (int i = 0; i < num_segments; i++) {
      float t = (float)i / (float)(num_segments - 1) * size();
      auto p = spline_->getPosition(t);
      output.emplace_back(T(p[0], p[1], p[2]));
    }
    return output;
  }

  T getPointAt(float pct) const {
    if (spline_ == nullptr) return T(0, 0, 0);
    auto p = spline_->getWiggle(pct * size()).position;
    return T(p[0], p[1], p[2]);
  }

  T getTangentAt(float pct) const {
    if (spline_ == nullptr) return T(0, 0, 0);
    auto p = spline_->getWiggle(pct * size()).tangent;
    return T(p[0], p[1], p[2]);
  }

  T getCurvatureAt(float pct) const {
    if (spline_ == nullptr) return T(0, 0, 0);
    auto p = spline_->getWiggle(pct * size()).curvature;
    return T(p[0], p[1], p[2]);
  }

  T getWiggleAt(float pct) const {
    if (spline_ == nullptr) return T(0, 0, 0);
    auto p = spline_->getWiggle(pct * size()).wiggle;
    return T(p[0], p[1], p[2]);
  }

  float getLength() const {
    if (spline_ == nullptr) return 0;
    return spline_->totalLength();
  }

 protected:
  std::shared_ptr<Spline<T, float>> spline_;
};

using SplineCurve = BaseSplineCurve<glm::vec3>;

template <class T>
class BaseBSpline : public BaseSplineCurve<T> {
  using Points = std::vector<T>;

 public:
  bool generate(const Points& control_points, int degree = 4) {
    return generate(GENERIC_B_SPLINE, control_points, degree);
  }
};

using BSpline = BaseBSpline<glm::vec3>;

}  // namespace geom
}  // namespace limas
