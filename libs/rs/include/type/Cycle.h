#pragma once
#include "system/Exception.h"

namespace rs {

template <typename T>
class Cycle {
 private:
  std::deque<T> container_;
  size_t size_;
  size_t current_index_;

 public:
  explicit Cycle(size_t size) : size_(size), current_index_(0) {
    if (size == 0) {
      throw rs::Exception("Size should be greater than 0.");
    }
  }

  void push(const T& value) {
    if (container_.size() < size_) {
      container_.push_back(value);
    } else {
      container_[current_index_] = value;
      incrementIndex();
    }
  }

  void pop() {
    if (container_.empty()) {
      throw rs::Exception("The container_ is empty.");
    }
    container_.erase(container_.begin() + current_index_);
    if (current_index_ == container_.size() && !container_.empty()) {
      // If we removed the last element, adjust the index.
      current_index_ = 0;
    }
  }

  const T& current() const {
    if (container_.empty()) {
      throw rs::Exception("The container is empty.");
    }
    return container_[current_index_];
  }

  void incrementIndex() {
    if (container_.empty()) {
      throw rs::Exception("The container is empty.");
    }
    current_index_ = (current_index_ + 1) % size_;
  }

  void decrementIndex() {
    if (container_.empty()) {
      throw rs::Exception("The container is empty.");
    }
    current_index_ = (current_index_ == 0) ? size_ - 1 : current_index_ - 1;
  }
};

}  // namespace rs