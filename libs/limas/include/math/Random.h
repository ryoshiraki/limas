#pragma once
#include "system/Singleton.h"

namespace limas {
namespace math {

class Random {
  friend class Singleton<Random>;

 public:
  void randomize() { engine_.seed(std::random_device{}()); }
  void seedRandom(uint32_t seed) { engine_.seed(seed); }
  bool randb() { return engine_() & 1; }
  int32_t randi() { return engine_(); }
  uint32_t randui() { return engine_(); }

  int32_t randi(int32_t v) {
    if (v <= 0)
      return 0;
    else
      return engine_() % v;
  }

  uint32_t randui(uint32_t v) {
    if (v == 0)
      return 0;
    else
      return engine_() % v;
  }

  int32_t randi(int32_t a, int32_t b) { return randi(b - a) + a; }
  float randf() { return dist_(engine_); }
  float randf(float v) { return dist_(engine_) * v; }
  float randf(float a, float b) { return dist_(engine_) * (b - a) + a; }

  glm::vec3 rand3fv() {
    float phi = randf(glm::pi<float>() * 2.0f);
    float costheta = randf(-1.0f, 1.0f);

    float rho = std::sqrt(1.0f - costheta * costheta);
    float x = rho * std::cos(phi);
    float y = rho * std::sin(phi);
    float z = costheta;

    return glm::vec3(x, y, z);
  }

  glm::vec2 rand2fv() {
    float theta = randf(glm::pi<float>() * 2.0f);
    return glm::vec2(cos(theta), sin(theta));
  }

  float randGaussian() {
    static std::normal_distribution<float> dist{};
    return dist(engine_);
  }

  template <typename T>
  std::vector<int> randWeighted(const std::vector<T>& weights,
                                int num_samples) {
    std::discrete_distribution<> disc_dist(weights.begin(), weights.end());
    std::vector<int> samples(num_samples, 0);
    for_each(samples.begin(), samples.end(),
             [&](int& i) { i = disc_dist(engine_); });
    return samples;
  }

  template <typename T>
  void shuffle(std::vector<T>& input) {
    std::shuffle(input.begin(), input.end(), engine_);
  }

 private:
  Random() { engine_.seed(std::random_device{}()); }

  std::mt19937 engine_;
  std::uniform_real_distribution<float> dist_;
};

inline void seedRandom(uint32_t seed) {
  Singleton<Random>::getInstance().seedRandom(seed);
}
inline bool randBool() { return Singleton<Random>::getInstance().randb(); }
inline int randInt() { return Singleton<Random>::getInstance().randi(); }
inline int randInt(int v) { return Singleton<Random>::getInstance().randi(v); }
inline int randInt(int a, int b) {
  return Singleton<Random>::getInstance().randi(a, b);
}
inline float randFloat() { return Singleton<Random>::getInstance().randf(); }
inline float randFloat(float v) {
  return Singleton<Random>::getInstance().randf(v);
}
inline float randFloat(float a, float b) {
  return Singleton<Random>::getInstance().randf(a, b);
}

inline glm::vec2 randVec2() {
  return Singleton<Random>::getInstance().rand2fv();
}

inline glm::vec3 randVec3() {
  return Singleton<Random>::getInstance().rand3fv();
}

inline float randGaussian() {
  return Singleton<Random>::getInstance().randGaussian();
}

template <typename T>
inline std::vector<int> randWeighted(const std::vector<T>& weights,
                                     int num_samples) {
  return Singleton<Random>::getInstance().randWeighted<T>(weights, num_samples);
}

inline glm::vec2 randInCircle() {
  auto p = randVec2();
  float r = randFloat();
  r = std::cbrt(r);
  return {p.x * r, p.y * r};
}

inline glm::vec3 randInSphere() {
  auto p = randVec3();
  float r = randFloat();
  r = std::cbrt(r);
  return {p.x * r, p.y * r, p.z * r};
}

template <typename T>
inline void shuffle(std::vector<T>& input) {
  Singleton<Random>::getInstance().shuffle<T>(input);
}

}  // namespace math
}  // namespace limas
