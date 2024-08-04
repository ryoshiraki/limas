#pragma once
#include "clipper.h"
#include "system/Singleton.h"

namespace limas {
namespace geom {

class BooleanOp {
  friend class Singleton<BooleanOp>;

 public:
  template <class T>
  static std::vector<std::vector<T>> getUnion(const std::vector<T>& subj,
                                              const std::vector<T>& clip,
                                              int precision = 4) {
    return Singleton<BooleanOp>::getInstance().execute(
        Clipper2Lib::ClipType::Union, subj, clip, precision);
  }

  template <class T>
  static std::vector<std::vector<T>> getIntersection(const std::vector<T>& subj,
                                                     const std::vector<T>& clip,
                                                     int precision = 4) {
    return Singleton<BooleanOp>::getInstance().execute(
        Clipper2Lib::ClipType::Intersection, subj, clip, precision);
  }

  template <class T>
  static std::vector<std::vector<T>> getXor(const std::vector<T>& subj,
                                            const std::vector<T>& clip,
                                            int precision = 4) {
    return Singleton<BooleanOp>::getInstance().execute(
        Clipper2Lib::ClipType::Xor, subj, clip, precision);
  }

  template <class T>
  static std::vector<std::vector<T>> getDifference(const std::vector<T>& subj,
                                                   const std::vector<T>& clip,
                                                   int precision = 4) {
    return Singleton<BooleanOp>::getInstance().execute(
        Clipper2Lib::ClipType::Difference, subj, clip, precision);
  }

 private:
  BooleanOp() {}

  template <class T>
  static Clipper2Lib::PathD toClipper(const std::vector<T>& src) {
    Clipper2Lib::PathD output(src.size());
    for (int i = 0; i < src.size(); i++) {
      output[i] = Clipper2Lib::PointD(src[i][0], src[i][1]);
    }
    return output;
  }

  template <class T>
  static std::vector<T> toT(const Clipper2Lib::PathD& src) {
    std::vector<T> output(src.size());
    for (int i = 0; i < src.size(); i++) {
      output[i][0] = src[i].x;
      output[i][1] = src[i].y;
    }
    return output;
  }

  template <class T>
  static std::vector<std::vector<T>> execute(Clipper2Lib::ClipType type,
                                             const std::vector<T>& subj,
                                             const std::vector<T>& clip,
                                             int precision) {
    Clipper2Lib::PathD c_subj;
    toClipper<T>(subj, &c_subj);
    Clipper2Lib::PathD c_clip;
    toClipper<T>(clip, &c_clip);

    Clipper2Lib::PathsD output = Clipper2Lib::BooleanOp(
        type, Clipper2Lib::FillRule::NonZero, {c_subj}, {c_clip}, precision);

    std::vector<std::vector<T>> result(output.size());
    for (int i = 0; i < output.size(); i++) {
      result[i] = toT<T>(output[i]);
    }

    return result;
  }
};

}  // namespace geom
}  // namespace limas
