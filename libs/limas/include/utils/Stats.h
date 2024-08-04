#pragma once
#include "utils/Stopwatch.h"

#ifdef __APPLE__
#include <mach/mach.h>
#endif

#define TVAL2MSEC(tval) ((tval.seconds * 1000) + (tval.microseconds / 1000))
#define TVAL2SEC(tval) ((tval.seconds) + (tval.microseconds / 1000000.0))

namespace limas {

class Stats {
  struct TimeInfo {
    double wall_time;
    double user_cpu_time;
    double system_cpu_time;
  };

 public:
  Stats() : frame_time_({0, 0, 0}), last_time_({0, 0, 0}) {}

  void begin() {
    if (!stopwatch_.isRunning())
      stopwatch_.start();
    else {
      last_time_.wall_time = stopwatch_.getElapsedInSeconds();
      auto cpu_time = getCpuTime();
      last_time_.user_cpu_time = cpu_time.first;
      last_time_.system_cpu_time = cpu_time.second;
    }
  }

  void end() {
    auto wall_time = stopwatch_.getElapsedInSeconds();
    auto cpu_time = getCpuTime();
    frame_time_.wall_time = wall_time - last_time_.wall_time;
    frame_time_.user_cpu_time = cpu_time.first - last_time_.user_cpu_time;
    frame_time_.system_cpu_time = cpu_time.second - last_time_.system_cpu_time;
  }

  double getWallTimeInMs() const { return frame_time_.wall_time * 1000.0; }
  double getUserCpuTimeInMs() const { return frame_time_.user_cpu_time; }
  double getSystemCpuTimeInMs() const { return frame_time_.system_cpu_time; }
  double getCpuTimeInMs() const {
    return (frame_time_.user_cpu_time + frame_time_.system_cpu_time);
  }
  double getCpuUsageInPerc() const {
    return (frame_time_.user_cpu_time + frame_time_.system_cpu_time) /
           (frame_time_.wall_time * 1000.0);
  }
  double getMemoryUsageInMb() const { return _getMemoryUsageInMb(); }

 protected:
#ifdef __APPLE__
  std::pair<double, double> getCpuTime() {
    struct task_thread_times_info time_info;
    mach_msg_type_number_t time_info_size = TASK_THREAD_TIMES_INFO_COUNT;
    kern_return_t status = task_info(current_task(), TASK_THREAD_TIMES_INFO,
                                     (task_info_t)&time_info, &time_info_size);
    if (status != KERN_SUCCESS) return {0, 0};

    double user_cpu_time = TVAL2MSEC(time_info.user_time);
    double system_cpu_time = TVAL2MSEC(time_info.system_time);

    return std::make_pair(user_cpu_time, system_cpu_time);
  }

  double _getMemoryUsageInMb() const {
    struct task_basic_info basic_info;
    mach_msg_type_number_t basic_info_size = TASK_BASIC_INFO_COUNT;
    kern_return_t status =
        task_info(current_task(), TASK_BASIC_INFO, (task_info_t)&basic_info,
                  &basic_info_size);
    if (status != KERN_SUCCESS) return 0;

    return (basic_info.resident_size) / 1024.0 / 1024.0;
    // user_time_ = tval2msec(basic_info.user_time);
    // system_time_ = tval2msec(basic_info.system_time);
  }

#else
// TODO: add windows version
#endif

  TimeInfo frame_time_;
  TimeInfo last_time_;
  PreciseStopwatch stopwatch_;
};

}  // namespace limas