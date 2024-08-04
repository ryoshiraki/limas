#pragma once

namespace rs {

template <typename T>
class Singleton {
 public:
  Singleton(Singleton const&) = delete;
  Singleton(Singleton&&) = delete;
  Singleton& operator=(Singleton const&) = delete;
  Singleton& operator=(Singleton&&) = delete;

  static T& getInstance() {
    std::call_once(init_flag_, &Singleton::init);
    return *instance_;
  }

 private:
  Singleton() {}
  ~Singleton() {}

  static void init() { instance_.reset(new T); }

  static std::unique_ptr<T> instance_;
  static std::once_flag init_flag_;
};

template <typename T>
std::unique_ptr<T> Singleton<T>::instance_;
template <typename T>
std::once_flag Singleton<T>::init_flag_;

}  // namespace rs
