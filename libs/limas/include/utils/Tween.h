#pragma once
#include "math/Easing.h"
#include "utils/Timer.h"

namespace limas {

template <typename Clock>
class BaseTween : public BaseTimer<Clock> {
 protected:
  using typename BaseTimer<Clock>::Duration;

 public:
  template <class T>
  class Node : public BaseTimer<Clock>::Node {
    friend class BaseTween<Clock>;

   public:
    using Ptr = std::shared_ptr<Node>;

    Node() = delete;
    Node(const T& start, const T& end, Duration start_time, Duration duration,
         const std::function<float(float)>& easing = math::easing::none,
         const std::function<void(const T&)>& update_callback = nullptr,
         const std::function<void(void)>& finish_callback = nullptr)
        : BaseTimer<Clock>::Node(start_time, duration, finish_callback),
          value_(start),
          start_(start),
          end_(end),
          update_callback_(update_callback),
          easing_(easing) {}

    T get() const { return value_; }

   protected:
    virtual void update(Duration time) override {
      BaseTimer<Clock>::Node::update(time);
      value_ = start_ + (end_ - start_) * easing_(this->position_);
      if (update_callback_) {
        update_callback_(value_);
      }
    }

    T value_;
    T start_, end_;
    std::function<void(const T&)> update_callback_;
    std::function<float(float)> easing_;
  };

  BaseTween() {}

  template <class T>
  typename Node<T>::Ptr add(
      const std::string& key, const boost::type_identity_t<T>& start,
      const boost::type_identity_t<T>& end, Duration duration,
      const std::function<float(float)>& easing = math::easing::none,
      const std::function<void(const T&)>& update_callback = nullptr,
      const std::function<void(void)>& finish_callback = nullptr) {
    if (this->nodes_.find(key) != this->nodes_.end()) {
      logger::warn("Tween") << key << " is ready set." << logger::end();
      return nullptr;
    }
    auto node =
        std::make_shared<Node<T>>(start, end, this->getTime(), duration, easing,
                                  update_callback, finish_callback);
    this->nodes_.insert(std::make_pair(key, node));
    return node;
  }

  template <class T>
  typename Node<T>::Ptr add(
      const boost::type_identity_t<T>& start,
      const boost::type_identity_t<T>& end, Duration duration,
      const std::function<float(float)>& easing = math::easing::none,
      const std::function<void(const T&)>& update_callback = nullptr,
      const std::function<void(void)>& finish_callback = nullptr) {
    std::string key = "node_" + std::to_string(count_++);
    return add<T>(key, start, end, duration, easing, update_callback,
                  finish_callback);
  }

  template <class T>
  T get(const std::string& key) {
    auto node = this->getNode(key);
    if (node != nullptr)
      return std::dynamic_pointer_cast<Node<T>>(node)->get();
    else
      return T();
  }

  uint32_t count_ = 0;
};

using Tween = BaseTween<std::chrono::system_clock>;
using PreciseTween = BaseTween<std::chrono::high_resolution_clock>;
using SteadyTween = BaseTween<std::chrono::steady_clock>;
}  // namespace limas