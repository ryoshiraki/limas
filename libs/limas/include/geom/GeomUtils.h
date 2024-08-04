#pragma once
#include "glm/gtx/exterior_product.hpp"  //for cross product of vec2
#include "math/Math.h"

namespace limas {
namespace geom {
using namespace std;

inline float getSignedArea(const std::vector<glm::vec2>& polygon) {
  float signed_area = 0.0f;
  for (size_t i = 0; i < polygon.size(); ++i) {
    const T& v1 = polygon[i];
    const T& v2 = polygon[(i + 1) % polygon.size()];
    signed_area += (v1.x * v2.y) - (v2.x * v1.y);
  }
  return signed_area;
}

inline bool getArea(const std::vector<glm::vec2>& polygon) {
  return abs(getSignedArea(polygon));
}

inline bool isClockwise(const std::vector<glm::vec2>& polygon) {
  return getSignedArea(polygon) < 0.0f;
}

static bool getIntersection(const glm::vec2& u0, const glm::vec2& u1,
                            const glm::vec2& v0, const glm::vec2& v1,
                            glm::vec2* res) {
  double s, t;
  double deno = glm::cross(v1 - v0, u1 - u0);

  if (deno == 0.0) {
    return false;
  }

  s = glm::cross(u0 - v0, u1 - u0) / deno;
  t = glm::cross(v1 - v0, v0 - u0) / deno;
  if (s < 0.0 || 1.0 < s || t < 0.0 || 1.0 < t) {
    return false;
  }

  if (res != nullptr) {
    (*res)[0] = v0.x + s * (v1 - v0).x;
    (*res)[1] = v0.y + s * (v1 - v0).y;
  }
  return true;
}

static bool isPointInside(const glm::vec2& p,
                          const std::vector<glm::vec2>& points) {
  int i, j, n = points.size();
  bool c = false;

  for (i = 0, j = n - 1; i < n; j = i++) {
    if (((points[i].y >= p.y) != (points[j].y >= p.y)) &&
        (p.x <= (points[j].x - points[i].x) * (p.y - points[i].y) /
                        (points[j].y - points[i].y) +
                    points[i].x))
      c = !c;
  }

  return c;
}

inline glm::mat4 getObliqueNearPlaneClipping(const glm::vec4& plane,
                                             const glm::mat4& view,
                                             const glm::mat4& proj) {
  glm::vec4 clip_plane = glm::transpose(glm::inverse(view)) * plane;  // plane;
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

inline glm::vec3 getXAxis(const glm::mat4& view) {
  glm::mat4 inv_view = glm::inverse(view);
  return glm::vec3(inv_view[0]);
}

inline glm::vec3 getYAxis(const glm::mat4& view) {
  glm::mat4 inv_view = glm::inverse(view);
  return glm::vec3(inv_view[1]);
}

inline glm::vec3 getZAxis(const glm::mat4& view) {
  glm::mat4 inv_view = glm::inverse(view);
  return glm::vec3(inv_view[2]);
}

inline float getAngleBetween(const glm::vec2& v0, const glm::vec2& v1) {
  float dot = glm::dot(v0, v1);
  float mag = glm::length(v0) * glm::length(v1);
  float v = dot / mag;
  float rad = std::acos(v);
  return rad;
}

inline float getAngle(const glm::vec2& v) { return std::atan2(v.y, v.x); }

inline glm::quat getRotationBetween(const glm::vec3& _from,
                                    const glm::vec3& _to) {
  from = glm::normalize(_from);
  to = glm::normalize(_to);

  float cos_theta = glm::dot(from, to);
  glm::vec3 axis;

  if (cos_theta < -1 + math::eps()) {
    axis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), from);
    if (math::length2(axis) < 0.01)
      axis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), from);

    axis = glm::normalize(axis);
    return glm::angleAxis(glm::radians(180.0f), axis);
  }

  axis = glm::cross(from, to);

  float s = sqrt((1 + cos_theta) * 2);
  float invs = 1 / s;

  return glm::quat(s * 0.5f, axis.x * invs, axis.y * invs, axis.z * invs);
}

}  // namespace geom
}  // namespace limas
