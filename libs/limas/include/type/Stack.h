#pragma once

namespace limas {

template <class T>
class Stack {
 public:
  Stack() {}

  T& get() { return states_.top(); }

  Stack& push() {
    states_.emplace(get());
    return *this;
  }

  Stack& pop() {
    states_.pop();
    assert(!states_.empty());
    return *this;
  }

  Stack& push(const T& t) {
    states_.push(t);
    return *this;
  }

 private:
  std::stack<T> states_;
};

}  // namespace limas