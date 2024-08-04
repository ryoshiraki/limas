#pragma once
#include "system/Exception.h"

namespace rs {

class Noncopyable : private boost::noncopyable {
 public:
  Noncopyable() {}

  Noncopyable(const Noncopyable&) {
    throw rs::Exception("Copy construction of Noncopyable is not allowed.");
  }

  Noncopyable& operator=(const Noncopyable&) {
    throw rs::Exception("Copy assignment of Noncopyable is not allowed.");
    return *this;
  }
};

}  // namespace rs
