#pragma once
#include "math/Math.h"

namespace limas {
namespace math {

class Drunk {
 private:
  int value_;
  int step_;
  int min_;
  int max_;
  bool has_min_;
  bool has_max_;
  bool allow_duplicate_;

  std::mt19937 engine_;
  std::uniform_int_distribution<int> dist_;

 public:
  Drunk() : Drunk(0, 1) {}

  Drunk(int initial, int step, const std::optional<int>& min = {},
        const std::optional<int>& max = {})
      : value_(initial),
        step_(step),
        dist_(-step, step),
        allow_duplicate_(true) {
    engine_.seed(std::random_device{}());

    if (min) {
      min_ = *min;
      has_min_ = true;
    }

    if (max) {
      max_ = *max;
      has_max_ = true;
    }
  }

  int getValue() {
    int new_value;

    do {
      new_value = value_ + dist_(engine_);
      if (has_max_ && new_value > max_) new_value = max_;
      if (has_min_ && new_value < min_) new_value = min_;
    } while (!allow_duplicate_ && new_value == value_);

    value_ = new_value;
    return value_;
  }

  void setValue(int value) { value_ = value; }

  void setStepSize(int step) {
    step_ = step;
    dist_ = std::uniform_int_distribution<int>(-step_, step_);
  }

  void setMin(int min) {
    min_ = min;
    has_min_ = true;
  }

  void setMax(int max) {
    max_ = max;
    has_max_ = true;
  }

  void setAllowDuplicate(bool allow) { allow_duplicate_ = allow; }
};
}  // namespace math
}  // namespace limas