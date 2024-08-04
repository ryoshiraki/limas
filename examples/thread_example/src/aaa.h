using Locker = std::unique_lock<std::mutex>;

class Thread : private Noncopyable {
 public:
  Thread() : b_running_(false), b_should_stop_(false) {}
  virtual ~Thread() {
    stopThread();
    joinThread();
  }

  void startThread() {
    if (!b_running_ && b_should_stop_) {
      stopThread();
      joinThread();
    }

    auto locker = getLock();
    if (b_running_) return;

    b_running_ = true;
    b_should_stop_ = false;
    thread_ = std::thread(&Thread::_threadedFunction, this);
  }

  void joinThread() {
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  void stopThread() {
    {
      auto locker = getLock();
      b_should_stop_ = true;
    }
    notify();
  }

  bool isThreadRunning() const { return b_running_ && !b_should_stop_; }

 protected:
  virtual void threadedFunction() = 0;

  void lock() const { mutex_.lock(); }
  void unlock() const { mutex_.unlock(); }

  Locker getLock() const {
    Locker locker(mutex_);
    return std::move(locker);
  }

  void sleepFor(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }

  template <typename Predicate>
  void waitFor(Locker& locker, Predicate predicate) {
    cv_.wait(locker, predicate);
  }

  void notify() { cv_.notify_one(); }

  std::condition_variable cv_;
  mutable std::mutex mutex_;

 private:
  void _threadedFunction() {
    threadedFunction();
    b_running_ = false;
  }

  std::thread thread_;
  std::atomic<bool> b_running_;
  std::atomic<bool> b_should_stop_;
};