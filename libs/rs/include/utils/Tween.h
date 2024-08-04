#pragma once
#include "math/Easing.h"
#include "utils/Timer.h"

namespace rs {

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
         bool b_loop,
         const std::function<float(float)>& easing = math::easing::none,
         const std::function<void(void)>& callback = 0)
        : BaseTimer<Clock>::Node(start_time, duration, b_loop, callback),
          value_(start),
          start_(start),
          end_(end),
          easing_(easing) {}

    T get() const { return value_; }

   protected:
    virtual void update(Duration time) override {
      BaseTimer<Clock>::Node::update(time);
      value_ = start_ + (end_ - start_) * easing_(this->position_);
    }

    T value_;
    T start_, end_;
    std::function<float(float)> easing_;
  };

  BaseTween() {}

  template <class T>
  typename Node<T>::Ptr add(
      const std::string& key, const boost::type_identity_t<T>& start,
      const boost::type_identity_t<T>& end, Duration duration,
      bool b_loop = false,
      const std::function<float(float)>& easing = math::easing::none,
      const std::function<void(void)>& callback = nullptr) {
    if (this->nodes_.find(key) != this->nodes_.end()) {
      log::warn("Tween") << key << " is ready set." << log::end();
      return nullptr;
    }
    auto node = std::make_shared<Node<T>>(start, end, this->getTime(), duration,
                                          b_loop, easing, callback);
    this->nodes_.insert(std::make_pair(key, node));
    return node;
  }

  template <class T>
  T get(const std::string& key) {
    auto node = this->getNode(key);
    if (node != nullptr)
      return std::dynamic_pointer_cast<Node<T>>(node)->get();
    else
      return T();
  }
};

using Tween = BaseTween<std::chrono::system_clock>;
using PreciseTween = BaseTween<std::chrono::high_resolution_clock>;
using SteadyTween = BaseTween<std::chrono::steady_clock>;
}  // namespace rs