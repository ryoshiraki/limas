#pragma once
#include "system/Exception.h"
#include "system/Logger.h"
#include "system/Noncopyable.h"

namespace limas {

using Locker = std::unique_lock<std::mutex>;

class Thread : private Noncopyable {
 public:
  Thread() : b_running_(false), b_should_stop_(false) {}
  virtual ~Thread() { stopThread(); }

  void startThread() {
    auto locker = getLock();
    if (b_running_) return;

    b_running_ = true;
    b_should_stop_ = false;
    thread_ = std::thread(&Thread::_threadedFunction, this);
  }

  void stopThread() {
    {
      auto locker = getLock();
      b_should_stop_ = true;
    }

    cv_.notify_all();
    if (thread_.joinable()) thread_.join();
  }

  bool isThreadRunning() const { return b_running_ && !b_should_stop_; }

 protected:
  virtual void threadedFunction() = 0;

  Locker getLock() const {
    Locker locker(mutex_);
    return locker;
  }

  void sleepFor(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }

  template <typename Predicate>
  void waitFor(Locker& locker, Predicate predicate) {
    cv_.wait(locker, [&]() { return predicate() || b_should_stop_.load(); });
  }

  void notify() { cv_.notify_one(); }

  std::condition_variable cv_;
  mutable std::mutex mutex_;

 private:
  void _threadedFunction() {
    try {
      threadedFunction();
    } catch (Exception& e) {
      log::error("Thread") << e.what() << log::end();
    }
    b_running_ = false;
  }

  std::thread thread_;
  std::atomic<bool> b_running_;
  std::atomic<bool> b_should_stop_;
};

}  // namespace limas
