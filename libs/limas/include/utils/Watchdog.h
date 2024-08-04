#pragma once
#include "app/Window.h"
#include "system/Logger.h"
#include "system/Observable.h"
#include "system/SubProcess.h"
#include "system/Thread.h"
#include "utils/Stopwatch.h"

namespace limas {

class Watchdog : public Thread {
 public:
  Watchdog() = delete;
  Watchdog(Window::Ptr& win, float timeout_in_seconds,
           std::function<void()> callback)
      : timeout_(timeout_in_seconds), callback_(callback) {
    conn_ = win->setOnLoop(this, &Watchdog::reset);
    stopwatch_.start();
    start();
  }

  Watchdog(Window::Ptr& win, float timeout_in_seconds)
      : timeout_(timeout_in_seconds) {
    callback_ = []() { command::reboot(); };
    conn_ = win->setOnLoop(this, &Watchdog::reset);
    stopwatch_.start();
    start();
  }

  ~Watchdog() { condition_.notify_one(); }

 private:
  Stopwatch stopwatch_;

  void reset(const EventArgs&) {
    Locker locker = getLock();
    stopwatch_.restart();
  }

  void threadedFunction() {
    while (isRunning()) {
      Locker locker = getLock();
      float elapsed = stopwatch_.getElapsedInSeconds();
      if (elapsed >= timeout_) {
        log::error("watchdog timer expired");
        stopwatch_.restart();
        callback_();
      }
    }
  }

  float timeout_;
  std::function<void()> callback_;
  Connection conn_;
};

}  // namespace limas
