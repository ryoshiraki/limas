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

  void startThread(std::function<void()> func) {
    auto locker = getLock();
    if (b_running_) return;

    b_running_ = true;
    b_should_stop_ = false;
    thread_ = std::thread([this, func]() {
      try {
        func();
      } catch (Exception& e) {
        log::error("Thread") << e.what() << log::end();
      }
      b_running_ = false;
    });
    thread_.detach();
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

  Locker getLock() const {
    Locker locker(mutex_);
    return locker;
  }

  void sleepFor(uint16_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }

  template <typename Predicate>
  void waitFor(Locker& locker, Predicate predicate) {
    cv_.wait(locker, [&]() { return predicate() || b_should_stop_.load(); });
  }

 protected:
  void notify() { cv_.notify_one(); }

  std::condition_variable cv_;
  mutable std::mutex mutex_;

 private:
  std::thread thread_;
  std::atomic<bool> b_running_;
  std::atomic<bool> b_should_stop_;
};

}  // namespace limas
