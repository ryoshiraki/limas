#pragma once

namespace rs {

template <typename T>
struct IsVector : std::false_type {};

template <>
struct IsVector<glm::vec2> : std::true_type {};

template <>
struct IsVector<glm::vec3> : std::true_type {};

template <>
struct IsVector<glm::vec4> : std::true_type {};

template <typename T>
class BoundedValue {
 public:
  BoundedValue() : value_(T()), has_min_(false), has_max_(false) {}
  BoundedValue(const T& value)
      : value_(value), has_min_(false), has_max_(false) {}

  void setMin(const T& min) {
    min_ = min;
    has_min_ = true;
    clamp();
  }

  void setMax(const T& max) {
    max_ = max;
    has_max_ = true;
    clamp();
  }

  void setValue(const T& value) {
    value_ = value;
    clamp();
  }

  void clearMin() { has_min_ = false; }
  void clearMax() { has_max_ = false; }

  const T& getMin() const { return min_; }
  const T& getMax() const { return max_; }
  const T& getValue() const { return value_; }

  bool hasMin() const { return has_min_; }
  bool hasMax() const { return has_max_; }

  BoundedValue<T>& operator=(const T& value) {
    setValue(value);
    return *this;
  }
  operator T() const { return value_; }

 private:
  void clamp() {
    if constexpr (IsVector<T>::value) {
      clampVector();
    } else {
      clampScalar();
    }
  }

  void clampScalar() {
    if (has_min_ && value_ < min_) value_ = min_;
    if (has_max_ && value_ > max_) value_ = max_;
  }

  void clampVector() {
    for (int i = 0; i < T::length(); ++i) {
      if (has_min_) value_[i] = std::max(value_[i], min_[i]);
      if (has_max_) value_[i] = std::min(value_[i], max_[i]);
    }
  }

  T value_;
  T min_;
  T max_;
  bool has_min_;
  bool has_max_;
};
}  // namespace rs
