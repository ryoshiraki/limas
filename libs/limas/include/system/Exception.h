#pragma once

namespace limas {

class Exception : public std::exception {
 public:
  explicit Exception(const std::string& msg) : msg_(msg) {}
  virtual ~Exception() noexcept {}

  virtual const char* what() const noexcept { return msg_.c_str(); }

 private:
  std::string msg_;
};

}  // namespace limas
