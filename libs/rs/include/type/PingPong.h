#pragma once

namespace rs {

template <class T>
class PingPong {
 public:
  PingPong() : flag_(0) {
    flag_ = 0;
    swap();
    flag_ = 0;
  }
  virtual ~PingPong() {}

  T& getFront() const { return *front_; }

  T& getBack() const { return *back_; }

  void swap() {
    front_ = &(bodies_[(flag_) % 2]);
    back_ = &(bodies_[++(flag_) % 2]);
  } 

  T& operator[](int n) { return bodies_[n]; }

 protected:
  T bodies_[2];
  T* front_;
  T* back_;
  unsigned int flag_;
};

}  // namespace rs
