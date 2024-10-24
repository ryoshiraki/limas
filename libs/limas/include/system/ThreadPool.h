#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "system/Logger.h"
#include "system/Noncopyable.h"

namespace limas {

using Locker = std::unique_lock<std::mutex>;

class ThreadPool : private Noncopyable {
 public:
  ThreadPool(size_t numThreads) : b_should_stop_(false) { start(numThreads); }

  ~ThreadPool() { stop(); }

  // タスクを追加する
  template <typename F>
  void enqueue(F&& task) {
    {
      auto locker = getLock();
      tasks_.emplace(std::forward<F>(task));
    }
    cv_.notify_one();
  }

  // スレッドプールを停止してすべてのスレッドを終了させる
  void stop() {
    {
      auto locker = getLock();
      b_should_stop_ = true;
    }
    cv_.notify_all();
    for (auto& thread : threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
    threads_.clear();
  }

  bool isRunning() const { return !b_should_stop_; }

 private:
  std::vector<std::thread> threads_;
  std::queue<std::function<void()>> tasks_;
  mutable std::mutex mutex_;
  std::condition_variable cv_;
  std::atomic<bool> b_should_stop_;

  Locker getLock() const { return Locker(mutex_); }

  void start(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
      threads_.emplace_back([this] {
        while (true) {
          std::function<void()> task;

          {
            auto locker = getLock();
            cv_.wait(locker,
                     [this] { return !tasks_.empty() || b_should_stop_; });

            if (b_should_stop_ && tasks_.empty()) {
              return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
          }

          try {
            task();
          } catch (const std::exception& e) {
            log::error("ThreadPool") << e.what() << log::end();
          }
        }
      });
    }
  }
};

}  // namespace limas
