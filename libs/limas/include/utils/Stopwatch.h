#pragma once
#include <chrono>
#include <iomanip>
#include <iostream>

namespace limas {

template <typename T>
inline void printDuration(T dur) {
  using namespace std::chrono;
  auto h = duration_cast<duration<int, std::ratio<3600>>>(dur);
  dur -= h;
  auto m = duration_cast<duration<int, std::ratio<60>>>(dur);
  dur -= m;
  auto s = duration_cast<duration<int, std::ratio<1>>>(dur);
  dur -= s;
  //  auto ms = duration_cast<duration<double, std::milli>>(dur);
  //  dur -= ms;
  auto ns = duration_cast<duration<long long, std::nano>>(dur);

  std::cout << std::setfill('0') << std::right << std::setw(2) << h.count()
            << " " << std::setfill('0') << std::right << std::setw(2)
            << m.count() << " " << std::setfill('0') << std::right
            << std::setw(2) << s.count() << " " << std::setfill('0')
            << std::left
            << std::setw(6)
            //  << ms.count() << "\n";
            << ns.count() << "\n";
}

template <typename Clock>
class BaseStopwatch {
 public:
  using TimePoint = std::chrono::time_point<Clock>;
  using Duration = std::chrono::nanoseconds;

  BaseStopwatch() : b_running_(false), elpased_time_(Duration::zero()) {}

  virtual ~BaseStopwatch() {}

  virtual void start() {
    if (b_running_) return;
    b_running_ = true;
    start_time_ = Clock::now();
  }

  virtual void stop() {
    if (!b_running_) return;
    b_running_ = false;
    elpased_time_ += Duration(Clock::now() - start_time_);
  }

  virtual void reset() {
    b_running_ = false;
    elpased_time_ = Duration::zero();
  }

  virtual void restart() {
    b_running_ = true;
    start_time_ = Clock::now();
  }

  bool isRunning() const { return b_running_; }

  Duration getElapsed() const {
    return b_running_ ? (elpased_time_ + (Clock::now() - start_time_))
                      : elpased_time_;
  }

  double getElapsedInHours() const {
    return getElapsedAs<double, std::ratio<3600>>();
  }

  double getElapsedInMinutes() const {
    return getElapsedAs<double, std::ratio<60>>();
  }

  double getElapsedInSeconds() const {
    return getElapsedAs<double, std::ratio<1>>();
  }

  double getElapsedInMilliseconds() const {
    return getElapsedAs<double, std::milli>();
  }

  double getElapsedInMicroseconds() const {
    return getElapsedAs<double, std::micro>();
  }

  long long getElapsedInNanoseconds() const {
    return getElapsedAs<long long, std::nano>();
  }

 protected:
  template <typename Unit, class Ratio>
  Unit getElapsedAs() const {
    using namespace std::chrono;
    return duration_cast<duration<Unit, Ratio>>(getElapsed()).count();
  }

  bool b_running_;
  TimePoint start_time_;
  Duration elpased_time_;
};

using Stopwatch = BaseStopwatch<std::chrono::system_clock>;
using PreciseStopwatch = BaseStopwatch<std::chrono::high_resolution_clock>;
using SteadyStopwatch = BaseStopwatch<std::chrono::steady_clock>;

class ScopedStopwatch : public PreciseStopwatch {
 public:
  ScopedStopwatch(const std::string& name) : PreciseStopwatch(), name_(name) {
    start();
  }

  ~ScopedStopwatch() {
    stop();
    printDuration(elpased_time_);
  }

  std::string name_;
};

}  // namespace limas
