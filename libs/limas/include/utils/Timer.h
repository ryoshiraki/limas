#pragma once
#include "utils/Stopwatch.h"

namespace limas {

template <typename Clock>
class BaseTimer {
 protected:
  // using Unit = double;
  // using Ratio = std::milli;
  // using Duration = std::chrono::duration<Unit, Ratio>;
  using Duration = std::chrono::microseconds;

 public:
  class Node {
    friend class BaseTimer<Clock>;

   public:
    using Ptr = std::shared_ptr<Node>;

    Node() = delete;
    Node(Duration start_time, Duration duration, bool b_loop,
         const std::function<void(void)>& callback = 0)
        : start_time_(start_time),
          elapsed_(Duration(0)),
          duration_(duration),
          b_loop_(b_loop),
          position_(0),
          callback_(callback) {}
    virtual ~Node() {}

    virtual void update(Duration now) {
      now = std::clamp(now, start_time_, start_time_ + duration_);
      elapsed_ = now - start_time_;
      position_ = static_cast<double>(elapsed_.count()) /
                  static_cast<double>(duration_.count());
    }
    Duration setDuration(Duration start_time, Duration duration) {
      start_time_ = start_time;
      duration_ = duration;
    }

    Duration getStartTime() const { return start_time_; }
    Duration getDuration() const { return duration_; }
    Duration getElapsedTime() const { return elapsed_; }
    double getPosition() const { return position_; }

    void call() {
      if (callback_) callback_();
    }

    Duration start_time_;
    Duration duration_;
    Duration elapsed_;
    double position_;
    bool b_loop_;
    std::function<void(void)> callback_;
  };

  BaseTimer() {}
  void start() { stopwatch.start(); }
  void stop() { stopwatch.stop(); }
  void clear() {
    nodes_.clear();
    stopwatch.reset();
  }

  bool isRunning() const { return stopwatch.isRunning(); }

  typename Node::Ptr add(const std::string& key, Duration duration, bool b_loop,
                         const std::function<void(void)>& callback = nullptr) {
    if (this->nodes_.find(key) != this->nodes_.end()) {
      log::warn("EventTimer") << key << " is ready set." << log::end();
      return nullptr;
    }
    auto node =
        std::make_shared<Node>(this->getTime(), duration, b_loop, callback);
    this->nodes_.insert(std::make_pair(key, node));
    return node;
  }

  typename Node::Ptr getNode(const std::string& key) const {
    const auto it = this->nodes_.find(key);
    if (it != this->nodes_.end()) {
      return it->second;
    }
    return nullptr;
  }

  double getPosition(const std::string& key) const {
    auto node = getNode(key);
    if (node != nullptr) return node->getPosition();
    return 0;
  }

  Duration getElapsedTime(const std::string& key) const {
    auto node = getNode(key);
    if (node != nullptr) return node->getElapsedTime();
    return Duration(0);
  }

  bool has(const std::string& key) {
    auto it = nodes_.find(key);
    if (it != nodes_.end()) {
      return true;
    }
    return false;
  }

  bool remove(const std::string& key) {
    auto it = nodes_.find(key);
    if (it != nodes_.end()) {
      nodes_.erase(it);
      return true;
    }
    return false;
  }

  void update() {
    if (!stopwatch.isRunning() || nodes_.empty()) return;

    auto time = getTime();

    auto it = nodes_.begin();
    for (; it != nodes_.end();) {
      auto& obj = it->second;
      obj->update(time);
      if (obj->position_ >= 1) {
        obj->call();
        if (obj->b_loop_) {
          obj->start_time_ = time;
        } else {
          it = nodes_.erase(it);
          continue;
        }
      }
      ++it;
    }
  }

 protected:
  Duration getTime() const {
    return std::chrono::duration_cast<Duration>(stopwatch.getElapsed());
  }

  BaseStopwatch<Clock> stopwatch;
  std::map<std::string, typename Node::Ptr> nodes_;
};

using Timer = BaseTimer<std::chrono::system_clock>;
using PreciseTimer = BaseTimer<std::chrono::high_resolution_clock>;
using Steadyimer = BaseTimer<std::chrono::steady_clock>;

}  // namespace limas
