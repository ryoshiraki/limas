#pragma once
#include "system/Exception.h"

namespace limas {

class Noncopyable : private boost::noncopyable {
 public:
  Noncopyable() {}

  Noncopyable(const Noncopyable&) {
    throw Exception("Copy construction of Noncopyable is not allowed.");
  }

  Noncopyable& operator=(const Noncopyable&) {
    throw Exception("Copy assignment of Noncopyable is not allowed.");
    return *this;
  }
};

}  // namespace limas
