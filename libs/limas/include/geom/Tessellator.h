#pragma once

#include "earcut.hpp"
#include "system/Singleton.h"

namespace limas {
namespace geom {

class Tessellator {
  friend class Singleton<Tessellator>;
  using Point = std::array<float, 2>;

 public:
  template <typename T>
  static std::vector<int> get(const std::vector<T>& polygon,
                              const std::vector<std::vector<T>>& holes =
                                  std::vector<std::vector<T>>()) {
    auto& tess = Singleton<Tessellator>::getInstance();
    tess.clear();
    tess.setPolygon(polygon);
    tess.addHole(holes);
    return tess.execute();
  }

 protected:
  Tessellator() {}

  void clear() {
    polygon_.clear();
    holes_.clear();
  }

  template <typename T>
  void setPolygon(const std::vector<T>& polygon) {
    if (polygon.size() <= 2) return;

    polygon_.clear();
    std::transform(cbegin(polygon), cend(polygon), std::back_inserter(polygon_),
                   [](T p) { return Point{p[0], p[1]}; });
  }

  template <typename T>
  void addHole(const std::vector<T>& _hole) {
    if (_hole.size() <= 2) return;

    std::vector<Point> hole;
    std::transform(cbegin(_hole), cend(_hole), std::back_inserter(hole),
                   [](T p) { return Point{p[0], p[1]}; });
    holes_.push_back(hole);
  }

  template <typename T>
  void addHole(const std::vector<std::vector<T>>& holes) {
    for (auto& h : holes) addHole(h);
  }

  std::vector<int> execute() const {
    std::vector<std::vector<Point>> data;
    data.push_back(polygon_);
    if (holes_.size()) data.insert(end(data), begin(holes_), end(holes_));
    return mapbox::earcut<int>(data);
  }

  std::vector<Point> polygon_;
  std::vector<std::vector<Point>> holes_;
};

}  // namespace geom
}  // namespace limas
