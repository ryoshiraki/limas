#pragma once
#include "math/Math.h"

namespace limas {
namespace math {

class Oscillator {
 private:
 public:
  Oscillator() : amp_(1), freq_(0), phase_(0) {}
  ~Oscillator() {}

  void setAmplitude(double amp) { amp_ = amp; }
  void setFrequency(double freq) { freq_ = freq; }
  void setPhase(double phase) { phase_ = phase; }

  double getAmplitude() const { return amp_; }
  double getFrequency() const { return freq_; }
  double getPhase() const { return phase_; }

  virtual double get(float time) = 0;

 protected:
  double amp_;
  double freq_;
  double phase_;
};

class SineWave : public Oscillator {
 public:
  double get(float sec) {
    return amp_ * sin(math::twopi() * freq_ * sec + phase_);
  }
};

class TriangleWave : public Oscillator {
 public:
  double get(float sec) {
    double period = 1.0 / freq_;
    double t = fmod(sec + phase_, period);
    double x = 2 * t / period;
    if (x < 1.0) {
      return amp_ * x;
    } else {
      return amp_ * (2.0 - x);
    };
  }
};

class SquareWave : public Oscillator {
 public:
  double get(float sec) {
    double period = 1.0 / freq_;
    double t = fmod(sec + phase_, period);
    return (t < period / 2) ? amp_ : -amp_;
  }
};

}  // namespace math
}  // namespace limas