#pragma once

namespace rs {
namespace math {

using namespace std;

// template<typename T> struct is_shared_ptr : std::false_type {};
// template<typename T> struct is_shared_ptr<std::shared_ptr<T>> :
// std::true_type {};

// template <class T>
// typename
// std::enable_if<is_shared_ptr<decltype(std::declval<T>().value)>::value,
// void>::type func( T t )
//{
//     std::cout << "shared ptr" << std::endl;
// }
//
// template <class T>
// typename
// std::enable_if<!is_shared_ptr<decltype(std::declval<T>().value)>::value,
// void>::type func( T t )
//{
//     std::cout << "non shared" << std::endl;
// }

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

  bool intersects(const Circle& circle, const Rect& rect) {
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

 public:
  using Ptr = std::shared_ptr<QuadTree<T>>;
  QuadTree() {}

  QuadTree(float x, float y, float w, float h, int capacity) {
    setup(x, y, w, h, capacity);
  }

  void setup(float x, float y, float w, float h, int capacity) {
    this->rect = Rect{x, y, w, h};
    this->capacity = capacity;
    clear();
  }

  void clear() {
    points.clear();
    nw = ne = se = sw = nullptr;
  }

  bool insert(const T& p) {
    if (!inside(rect, p)) return false;
    if (points.size() < capacity && nw == nullptr) {
      points.push_back(p);
      return true;
    }

    if (nw == nullptr) subdivide();

    if (nw->insert(p)) return true;
    ;
    if (ne->insert(p)) return true;
    ;
    if (sw->insert(p)) return true;
    ;
    if (se->insert(p)) return true;
    ;

    return false;
  }

  void subdivide() {
    float x = rect.x;
    float y = rect.y;
    float w = rect.w;
    float h = rect.h;

    nw = make_shared<QuadTree<T>>(x, y, w * 0.5, h * 0.5, capacity);
    ne = make_shared<QuadTree<T>>(x + w * 0.5, y, w * 0.5, h * 0.5, capacity);
    sw = make_shared<QuadTree<T>>(x, y + h * 0.5, w * 0.5, h * 0.5, capacity);
    se = make_shared<QuadTree<T>>(x + w * 0.5, y + h * 0.5, w * 0.5, h * 0.5,
                                  capacity);
  }

  std::vector<T> queryRange(float x, float y, float w, float h) {
    vector<T> result;

    Rect range{x, y, w, h};

    if (!intersects(rect, range)) return result;

    for (auto& p : points) {
      if (inside(range, p)) result.push_back(p);
    }

    if (nw == nullptr) return result;

    auto nw_points = nw->queryRange(x, y, w, h);
    auto ne_points = ne->queryRange(x, y, w, h);
    auto sw_points = sw->queryRange(x, y, w, h);
    auto se_points = se->queryRange(x, y, w, h);

    std::copy(nw_points.begin(), nw_points.end(), std::back_inserter(result));
    std::copy(ne_points.begin(), ne_points.end(), std::back_inserter(result));
    std::copy(sw_points.begin(), sw_points.end(), std::back_inserter(result));
    std::copy(se_points.begin(), se_points.end(), std::back_inserter(result));
    return result;
  }

  std::vector<T> queryRange(float cx, float cy, float radius) {
    std::vector<T> result;

    Circle circle{cx, cy, radius};

    if (!intersects(circle, rect)) return result;

    for (auto& p : points) {
      if (inside(circle, p)) result.push_back(p);
    }

    if (nw == nullptr) return result;

    auto nw_points = nw->queryRange(cx, cy, radius);
    auto ne_points = ne->queryRange(cx, cy, radius);
    auto sw_points = sw->queryRange(cx, cy, radius);
    auto se_points = se->queryRange(cx, cy, radius);

    std::copy(nw_points.begin(), nw_points.end(), std::back_inserter(result));
    std::copy(ne_points.begin(), ne_points.end(), std::back_inserter(result));
    std::copy(sw_points.begin(), sw_points.end(), std::back_inserter(result));
    std::copy(se_points.begin(), se_points.end(), std::back_inserter(result));
    return result;
  }

  void show() {
    if (nw == nullptr) {
      glBegin(GL_LINE_LOOP);
      glVertex2f(rect.x, rect.y);
      glVertex2f(rect.x + rect.w, rect.y);
      glVertex2f(rect.x + rect.w, rect.y + rect.h);
      glVertex2f(rect.x, rect.y + rect.h);
      glEnd();
    } else {
      nw->show();
      ne->show();
      sw->show();
      se->show();
    }
  }

  QuadTree<T>::Ptr nw, ne, sw, se;
  int capacity;
  Rect rect;
  std::vector<T> points;

 private:
  template <class U = T>
  bool inside(const Rect& b, U u,
              typename std::enable_if<std::is_pointer<U>::value>::type* = 0) {
    return inside(b, u->x, u->y);
  }

  //  template<class U = T>
  //  bool inside(const Rect & b, U u, typename
  //  std::enable_if<is_shared_ptr<decltype(std::declval<U>().value)>::value,
  //  void>::type* = 0) {
  //    return inside(b, u->x, u->y);
  //  }

  template <class U = T>
  bool inside(const Rect& b, U u,
              typename std::enable_if<!std::is_pointer<U>::value>::type* = 0) {
    return inside(b, u.x, u.y);
  }

  template <class U = T>
  bool inside(const Circle& c, U u,
              typename std::enable_if<std::is_pointer<U>::value>::type* = 0) {
    return ((c.x - u->x) * (c.x - u->x) + (c.y - u->y) * (c.y - u->y)) <
           c.r * c.r;
  }

  //  template<class U = T>
  //  bool inside(const Circle & c, U u, typename
  //  std::enable_if<is_shared_ptr<decltype(std::declval<U>().value)>::value,
  //  void>::type* = 0) {
  //    return ((c.x - u->x) * (c.x - u->x) + (c.y - u->y) * (c.y - u->y)) < c.r
  //    * c.r;
  //  }

  template <class U = T>
  bool inside(const Circle& c, U u,
              typename std::enable_if<!std::is_pointer<U>::value>::type* = 0) {
    return ((c.x - u.x) * (c.x - u.x) + (c.y - u.y) * (c.y - u.y)) < c.r * c.r;
  }
};

}  // namespace math
}  // namespace rs
