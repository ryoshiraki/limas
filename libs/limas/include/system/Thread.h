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

    try {
      thread_ = std::thread([this, func]() {
        try {
          func();
        } catch (Exception& e) {
          logger::error("Thread") << e.what() << logger::end();
        }
        b_running_.store(false, std::memory_order_release);
      });
    } catch (const std::system_error& e) {
      logger::error("Thread creation failed") << e.what() << logger::end();
      b_running_.store(false, std::memory_order_release);
      throw;
    }
  }

  void stopThread() {
    {
      auto locker = getLock();
      b_should_stop_.store(true, std::memory_order_release);
    }

    cv_.notify_all();
    if (thread_.joinable()) thread_.join();
  }

  bool isThreadRunning() const {
    return b_running_.load(std::memory_order_acquire) &&
           !b_should_stop_.load(std::memory_order_acquire);
  }

  Locker getLock() const { return Locker(mutex_); }

  void sleepFor(uint16_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }

  template <typename Predicate>
  void waitFor(Locker& locker, Predicate predicate) {
    cv_.wait(locker, [&]() { return predicate() || b_should_stop_.load(); });
  }

  void notify() { cv_.notify_one(); }
 protected:

  std::condition_variable cv_;
  mutable std::mutex mutex_;

 private:
  std::thread thread_;
  std::atomic<bool> b_running_;
  std::atomic<bool> b_should_stop_;
};

}  // namespace limas
