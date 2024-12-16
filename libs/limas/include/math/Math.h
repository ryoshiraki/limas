#pragma once
#include "math/Drunk.h"
#include "math/Noise.h"
#include "math/Random.h"

inline std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
  return os << glm::to_string(v);
}
inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
  return os << glm::to_string(v);
}
inline std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
  return os << glm::to_string(v);
}
inline std::ostream& operator<<(std::ostream& os, const glm::mat3& m) {
  return os << glm::to_string(m);
}
inline std::ostream& operator<<(std::ostream& os, const glm::mat4& m) {
  return os << glm::to_string(m);
}

namespace limas {
namespace math {

inline float pi() { return glm::pi<float>(); }
inline float twopi() { return glm::pi<float>() * 2.0f; }
inline float eps() { return glm::epsilon<float>(); }
inline float toDegree(float rad) { return rad * 180.0f / pi(); }
inline float toRadian(float deg) { return deg * pi() / 180.0f; }
inline glm::mat4 toMat(const glm::quat& quat) { return glm::mat4_cast(quat); }

template <typename T, typename L, typename H>
T clamp(T x, L lower, H upper) {
  T lower_casted = static_cast<T>(lower);
  T upper_casted = static_cast<T>(upper);

  if (lower_casted > upper_casted) {
    std::swap(lower_casted, upper_casted);
  }

  return std::max(lower_casted, std::min(x, upper_casted));
}

inline float map(float x, float x0, float x1, float y0, float y1,
                 bool b_clamp = false) {
  float y = y0 + (x - x0) * ((y1 - y0) / (x1 - x0));
  return b_clamp ? clamp(y, y0, y1) : y;
}

// deprecated
inline float lerp(float x, float x0, float x1, float y0, float y1,
                  bool b_clamp = false) {
  float y = y0 + (x - x0) * ((y1 - y0) / (x1 - x0));
  return b_clamp ? clamp(y, y0, y1) : y;
}

inline float fract(float x) { return x - std::floor(x); }

template <typename T>
inline float mix(T a, T b, float t) {
  return a * (1 - t) + b * t;
}

inline glm::mat4 getOrthoProjection(float width, float height,
                                    float near = -INT_MAX,
                                    float far = INT_MAX) {
  return glm::ortho(0.0f, width, height, 0.0f, near, far);
}

inline std::vector<int> getDivisors(int num) {
  std::vector<int> divisors;
  for (int i = 1; i <= num; ++i) {
    if (num % i == 0) {
      divisors.push_back(i);
    }
  }
  return divisors;
}

inline glm::vec3 getWorldToScreen(const glm::vec3& p,
                                  const glm::mat4& modelview,
                                  const glm::mat4& proj, float x, float y,
                                  float w, float h) {
  glm::vec4 viewport(x, y, w, h);
  auto sp = glm::project(p, modelview, proj, viewport);
  // sp[1] = h - sp[1];
  sp[1] = h - sp[1];
  return sp;
}

inline std::vector<int> getCommonDivisors(const std::vector<int>& nums) {
  int min_num = *min_element(nums.cbegin(), nums.cend());

  std::vector<int> min_divisors;
  for (int i = 1; i <= min_num; ++i) {
    if (min_num % i == 0) {
      min_divisors.push_back(i);
    }
  }

  std::vector<int> common_divisors;
  for (auto& div : min_divisors) {
    bool is_common = true;
    for (const auto& num : nums) {
      if (num % div != 0) {
        is_common = false;
        break;
      }
    }
    if (is_common) {
      common_divisors.push_back(div);
    }
  }

  return common_divisors;
}

}  // namespace math
}  // namespace limas
