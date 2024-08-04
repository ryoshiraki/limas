#pragma once
#include <OpenGL/OpenGL.h>

#if __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/CGLDevice.h>
#endif

namespace rs {
namespace gl {

#if __APPLE__

class ContextScope {
 public:
  ContextScope() {
    if (ctx_ == nullptr || pix_stuff_ == nullptr)
      std::cout << "ContextScope is not setup." << std::endl;
    CGLCreateContext(pix_stuff_, ctx_, &new_ctx_);
    CGLLockContext(new_ctx_);
    CGLSetCurrentContext(new_ctx_);
    CGLEnable(new_ctx_, kCGLCEMPEngine);
  }

  static void setup() {
    ctx_ = CGLGetCurrentContext();
    pix_stuff_ = CGLGetPixelFormat(ctx_);
  }

 private:
  CGLContextObj new_ctx_;

  static CGLContextObj ctx_;
  static CGLPixelFormatObj pix_stuff_;
};

CGLContextObj ContextScope::ctx_ = nullptr;
CGLPixelFormatObj ContextScope::pix_stuff_ = nullptr;

#else

class ContextScope {
 public:
  ContextScope() {}
  static void setup() {
    std::cout << "ContextScope only supports OSX." << std::endl;
  }
};

#endif

}  // namespace gl
}  // namespace rs
