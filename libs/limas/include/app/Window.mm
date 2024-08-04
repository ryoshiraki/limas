#include "app/Window.h"

#if __APPLE__
#include <Cocoa/Cocoa.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#include "GLFW/glfw3native.h"

using namespace rs;

void Window::setFullScreen(int monitor_index) {
  if (state_.b_fullscreen) return;
  [NSApp setPresentationOptions:NSApplicationPresentationHideMenuBar | NSApplicationPresentationHideDock];
  NSWindow * cocoaWindow = glfwGetCocoaWindow(window_);
  
  [cocoaWindow setStyleMask:NSWindowStyleMaskBorderless];
  
  int monitor_count;
  GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
  
  state_ = {getPosition(), getSize(), true};
  
  int xpos, ypos;
  glfwGetMonitorPos(monitors[monitor_index], &xpos, &ypos);
  float scale = getPixelScale();
  // Scale (if needed) to physical pixels size, since setWindowPosition
  // uses physical pixel dimensions. On HIDPI screens pixelScreenCoordScale
  // is likely to be 2, on "normal" screens pixelScreenCoordScale will be 1:
  xpos *= scale;
  ypos *= scale;
  setPosition(xpos, ypos);
  glm::vec2 screen_size = gl::getScreenSize(getMonitorIndex());
  setSize(screen_size.x, screen_size.y);
  setPosition(xpos, ypos);
  
  [cocoaWindow makeFirstResponder:cocoaWindow.contentView];
}

void Window::setMultiMonitorFullScreen() {
  if (state_.b_fullscreen) return;
  [NSApp setPresentationOptions:NSApplicationPresentationHideMenuBar | NSApplicationPresentationHideDock];
  NSWindow * cocoaWindow = glfwGetCocoaWindow(window_);
  
  [cocoaWindow setStyleMask:NSWindowStyleMaskBorderless];
  
  int monitor_count;
  GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
  
  state_ = {getPosition(), getSize(), true};

  int x_min = INT_MAX, y_min = INT_MAX;
  int x_max = -INT_MAX, y_max = -INT_MAX;
  for(int i = 0; i < monitor_count; i++) {
    const GLFWvidmode * desktop_mode = glfwGetVideoMode(monitors[i]);
    int x, y;
    glfwGetMonitorPos(monitors[i], &x, &y);
    x_min = x < x_min ? x : x_min;
    y_min = y < y_min ? y : y_min;
    x_max = x > x_max ? x : x_max;
    y_max = y > y_max ? y : y_max;
  }
  float scale = 1;//getPixelScale();
  setSize((x_max - x_min) * scale, (y_max - y_min) * scale);
  setPosition(x_min * scale, y_min * scale);
  
  [cocoaWindow makeFirstResponder:cocoaWindow.contentView];
}

void Window::setDefaultScreen() {
  if (!state_.b_fullscreen) return;
  [NSApp setPresentationOptions:NSApplicationPresentationDefault];
  NSWindow * cocoaWindow = glfwGetCocoaWindow(window_);
  [cocoaWindow setStyleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable];
  glm::vec2 pos = state_.pos;//getWindowPosition();
  glm::vec2 size = state_.size;//getWindowSize();
  setSize(size.x, size.y);
//  if (getFrameCount() > 0) setPosition(pos.x, pos.y);
  setPosition(pos.x, pos.y);
  [cocoaWindow makeFirstResponder:cocoaWindow.contentView];
  state_ = {getPosition(), getSize(), false};
}

#else
void Window::setFullScreen(int monitor_index) {}
void Window::setMultiMonitorFullScreen() {}
void Window::setDefaultScreen() {}
#endif
