#pragma once
#include <algorithm>
#include <memory>
#include <type_traits>
#include <vector>

namespace limas {
namespace math {

// スマートポインタかどうかを判定するメタプログラミング
template <typename T>
struct is_smart_ptr : std::false_type {};

template <typename T>
struct is_smart_ptr<std::shared_ptr<T>> : std::true_type {};

template <typename T>
struct is_smart_ptr<std::unique_ptr<T>> : std::true_type {};

template <class T>
class QuadTree {
  struct Rect {
    float x, y, w, h;
  };

  struct Circle {
    float x, y, r;
  };

  bool intersects(const Rect& rect1, const Rect& rect2) const {
    return (rect1.x < (rect2.x + rect2.w) && (rect1.x + rect1.w) > rect2.x &&
            rect1.y < (rect2.y + rect2.h) && (rect1.y + rect1.h) > rect2.y);
  }

  bool intersects(const Rect& rect, const Circle& circle) {
    float dist_x = abs(circle.x - rect.x - rect.w * 0.5);
    float dist_y = abs(circle.y - rect.y - rect.h * 0.5);

    if (dist_x > (rect.w * 0.5 + circle.r)) return false;
    if (dist_y > (rect.h * 0.5 + circle.r)) return false;

    if (dist_x <= (rect.w * 0.5)) return true;
    if (dist_y <= (rect.h * 0.5)) return true;

    float corner_dist_sq = (dist_x - rect.w * 0.5) * (dist_x - rect.w * 0.5) +
                           (dist_y - rect.h * 0.5) * (dist_y - rect.h * 0.5);
    return corner_dist_sq <= (circle.r * circle.r);
  }

  bool inside(const Rect& rect, float x, float y) const {
    return x >= rect.x && x <= (rect.x + rect.w) && y >= rect.y &&
           y <= (rect.y + rect.h);
  }

  bool inside(const Circle& circle, float x, float y) const {
    return std::pow(circle.x - x, 2) + std::pow(circle.y - y, 2) <=
           std::pow(circle.r, 2);
  }

 public:
  using Ptr = std::shared_ptr<QuadTree<T>>;

  QuadTree() {}

  QuadTree(float x, float y, float w, float h, int capacity) {
    setup(x, y, w, h, capacity);
  }

  void setup(float x, float y, float w, float h, int capacity) {
    this->rect = Rect{x, y, w, h};
    this->capacity = capacity;
  }

  void clear() {
    points.clear();
    nw = ne = se = sw = nullptr;
  }

  bool insert(const T& p) {
    if (!inside(rect, getX(p), getY(p))) return false;
    if (points.size() < capacity && nw == nullptr) {
      points.push_back(p);
      return true;
    }

    if (nw == nullptr) subdivide();  // ノードを4分割
    if (nw->insert(p)) return true;
    if (ne->insert(p)) return true;
    if (sw->insert(p)) return true;
    if (se->insert(p)) return true;

    return false;
  }

  void subdivide() {
    float x = rect.x;
    float y = rect.y;
    float w = rect.w;
    float h = rect.h;

    nw = std::make_shared<QuadTree<T>>(x, y, w * 0.5, h * 0.5, capacity);
    ne = std::make_shared<QuadTree<T>>(x + w * 0.5, y, w * 0.5, h * 0.5,
                                       capacity);
    sw = std::make_shared<QuadTree<T>>(x, y + h * 0.5, w * 0.5, h * 0.5,
                                       capacity);
    se = std::make_shared<QuadTree<T>>(x + w * 0.5, y + h * 0.5, w * 0.5,
                                       h * 0.5, capacity);
  }

  std::vector<T> queryRange(float x, float y, float w, float h) {
    std::vector<T> result;

    Rect range{x, y, w, h};

    if (!intersects(rect, range)) return result;

    for (const auto& p : points) {
      if (inside(range, getX(p), getY(p))) {
        result.push_back(p);
      }
    }

    if (nw == nullptr) return result;

    auto nw_points = nw->queryRange(x, y, w, h);
    auto ne_points = ne->queryRange(x, y, w, h);
    auto sw_points = sw->queryRange(x, y, w, h);
    auto se_points = se->queryRange(x, y, w, h);

    result.insert(result.end(), nw_points.begin(), nw_points.end());
    result.insert(result.end(), ne_points.begin(), ne_points.end());
    result.insert(result.end(), sw_points.begin(), sw_points.end());
    result.insert(result.end(), se_points.begin(), se_points.end());

    return result;
  }

  std::vector<T> queryRange(float cx, float cy, float radius) {
    std::vector<T> result;

    Circle circle{cx, cy, radius};

    if (!intersects(rect, circle)) return result;

    for (const auto& p : points) {
      if (inside(circle, getX(p), getY(p))) {
        result.push_back(p);
      }
    }

    if (nw == nullptr) return result;

    auto nw_points = nw->queryRange(cx, cy, radius);
    auto ne_points = ne->queryRange(cx, cy, radius);
    auto sw_points = sw->queryRange(cx, cy, radius);
    auto se_points = se->queryRange(cx, cy, radius);

    result.insert(result.end(), nw_points.begin(), nw_points.end());
    result.insert(result.end(), ne_points.begin(), ne_points.end());
    result.insert(result.end(), sw_points.begin(), sw_points.end());
    result.insert(result.end(), se_points.begin(), se_points.end());

    return result;
  }

  QuadTree<T>::Ptr nw, ne, sw, se;
  int capacity;
  Rect rect;
  std::vector<T> points;

 private:
  template <class U = T>
  auto getX(U& u) -> decltype(u->x, float{}) {
    return u->x;
  }

  template <class U = T>
  auto getX(const U& u) -> decltype(u.x, float{}) {
    return u.x;
  }

  template <class U = T>
  auto getY(U& u) -> decltype(u->y, float{}) {
    return u->y;
  }

  template <class U = T>
  auto getY(const U& u) -> decltype(u.y, float{}) {
    return u.y;
  }
};

}  // namespace math
}  // namespace limas
