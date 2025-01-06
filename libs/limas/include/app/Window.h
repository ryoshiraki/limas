#pragma once
#include "gl/GLUtils.h"
#include "system/Logger.h"
#include "system/Observable.h"

namespace limas {

class Window {
 public:
  using Ptr = std::shared_ptr<Window>;

  struct Settings {
    int x = 0;
    int y = 0;
    int width = 640;
    int height = 480;
    std::string title = "";
    int major = 4;
    int minor = 1;
    bool resizable = true;
    bool visible = true;
    bool decorated = true;
    bool floating = false;
    bool transparent = false;
    int red_bits = 8;
    int green_bits = 8;
    int blue_bits = 8;
    int alpha_bits = 8;
    int depth_bits = 24;
    int samples = 0;
    bool doublebuffering = true;
    bool hide_cursor = true;
  };
  Window(GLFWwindow* window, uint32_t index, bool b_doublebuffering)
      : window_(window),
        index_(index),
        draw_call_(0),
        b_doublebuffering_(b_doublebuffering) {
    state_ = {getPosition(), getSize(), false};
  }

  static Window::Ptr createWindow(const Settings& settings, uint32_t index,
                                  GLFWwindow* share_with = nullptr) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings.major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings.minor);
    if ((settings.major == 3 && settings.minor >= 2) || settings.major >= 4) {
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    if (settings.major >= 3) {
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    }
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);

    glfwWindowHint(GLFW_RESIZABLE, settings.resizable);
    glfwWindowHint(GLFW_VISIBLE, settings.visible);
    glfwWindowHint(GLFW_DECORATED, settings.decorated);
    glfwWindowHint(GLFW_RED_BITS, settings.red_bits);
    glfwWindowHint(GLFW_GREEN_BITS, settings.green_bits);
    glfwWindowHint(GLFW_BLUE_BITS, settings.blue_bits);
    glfwWindowHint(GLFW_ALPHA_BITS, settings.alpha_bits);
    glfwWindowHint(GLFW_DEPTH_BITS, settings.depth_bits);
    glfwWindowHint(GLFW_FLOATING, settings.floating);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, settings.transparent);
    glfwWindowHint(GLFW_SAMPLES, settings.samples);
    glfwWindowHint(GLFW_DOUBLEBUFFER, settings.doublebuffering);

    GLFWwindow* window =
        glfwCreateWindow(settings.width, settings.height, "", NULL, share_with);
    if (window == NULL) {
      logger::error("Window") << "cannot create window" << logger::end();
      return nullptr;
    }

    glfwSetWindowTitle(window, settings.title.c_str());
    glfwSetWindowPos(window, settings.x, settings.y);
    glfwSetInputMode(
        window, GLFW_CURSOR,
        settings.hide_cursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

    glfwSetKeyCallback(window, Window::keyCallback);
    glfwSetMouseButtonCallback(window, Window::mouseButtonCallback);
    glfwSetCursorPosCallback(window, Window::cursorPosCallback);
    glfwSetScrollCallback(window, Window::scrollCallback);
    glfwSetFramebufferSizeCallback(window, Window::frameBufferSizeCallback);
    glfwSetWindowRefreshCallback(window, Window::windowRefreshCallback);
    glfwSetWindowCloseCallback(window, Window::windowCloseCallback);
    glfwSetDropCallback(window, Window::fileDropCallback);

    auto w = std::make_shared<Window>(window, index, settings.doublebuffering);
    glfwSetWindowUserPointer(window, w.get());

    return w;
  }

  void setTitle(const std::string& title) {
    glfwSetWindowTitle(window_, title.c_str());
  }
  void setPosition(int x, int y) { glfwSetWindowPos(window_, x, y); }
  void setSize(int w, int h) { glfwSetWindowSize(window_, w, h); }

  uint32_t getIndex() const { return index_; }

  glm::vec2 getPosition() const {
    int x, y;
    glfwGetWindowPos(window_, &x, &y);
    return glm::vec2(x, y);
  }

  glm::vec2 getSize() const {
    int w, h;
    glfwGetFramebufferSize(window_, &w, &h);
    return glm::vec2(w, h);
  }
  int getWidth() const { return getSize()[0]; }
  int getHeight() const { return getSize()[1]; }

  glm::vec2 getMousePos() const {
    double x, y;
    glfwGetCursorPos(window_, &x, &y);
    return glm::vec2(x, y);
  }
  float getMouseX() const { return getMousePos()[0]; }
  float getMouseY() const { return getMousePos()[1]; }

  int getMouseButton() const {
    return glfwGetMouseButton(window_, MOUSE_BUTTON_LEFT)
               ? MOUSE_BUTTON_LEFT
               : (glfwGetMouseButton(window_, MOUSE_BUTTON_RIGHT)
                      ? MOUSE_BUTTON_RIGHT
                      : MOUSE_BUTTON_NONE);
  }

  float getPixelScale() const {
    int fw, ww;
    glfwGetFramebufferSize(window_, &fw, nullptr);
    glfwGetWindowSize(window_, &ww, nullptr);
    return (float)fw / (float)ww;
  }

  GLFWmonitor* getMonitor() const {
    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    auto pos = getPosition();
    for (int i = 0; i < count; i++) {
      int x, y;
      glfwGetMonitorPos(monitors[i], &x, &y);
      const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
      if (pos.x >= x && pos.y >= y && pos.x < (x + mode->width) &&
          pos.y < (y + mode->height)) {
        return monitors[i];
      }
    }
    return nullptr;
  }

  void readToPixels(std::vector<unsigned char>* pixels, int x, int y, int width,
                    int height) const {
    pixels->resize(width * height * 4);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                 pixels->data());
  }

  void setClipboard(const std::string& text) const {
    glfwSetClipboardString(window_, text.c_str());
  }

  std::string getClipboard() const {
    const char* clipboard = glfwGetClipboardString(window_);
    return clipboard ? clipboard : "";
  }

  void setFullScreen() {
    if (state_.b_fullscreen) return;

    GLFWmonitor* monitor = getMonitor();
    if (monitor == nullptr) return;

    state_ = {getPosition(), getSize(), true};

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(window_, monitor, 0, 0, mode->width, mode->height,
                         mode->refreshRate);
  }

  void setDefaultScreen() {
    if (!state_.b_fullscreen) return;
    glm::vec2 pos = state_.pos;
    glm::vec2 size = state_.size;
    glfwSetWindowMonitor(window_, NULL, pos.x, pos.y, size.x, size.y,
                         GLFW_DONT_CARE);

    state_ = {getPosition(), getSize(), false};
  }

  void toggleFullscreen() {
    state_.b_fullscreen ? setDefaultScreen() : setFullScreen();
  }

  void maximize() const { glfwMaximizeWindow(window_); }
  void minimize() const { glfwIconifyWindow(window_); }
  void restore() const { glfwRestoreWindow(window_); }
  void focus() const { glfwFocusWindow(window_); }

  void bind() {
    glfwMakeContextCurrent(window_);
    glViewport(0, 0, getWidth(), getHeight());
    EventArgs e;
    observables_.loop_begin.notify(e);
  }

  void draw() {
    if (draw_call_) draw_call_();
  }

  void unbind() {
    EventArgs e;
    observables_.loop_end.notify(e);
    b_doublebuffering_ ? glfwSwapBuffers(window_) : glFlush();
  }

  template <class AppClass>
  void setDrawCall(AppClass* app, void (AppClass::*func)(void)) {
    draw_call_ = std::bind(func, app);
  }
  void setDrawCall(const std::function<void(void)>& func) { draw_call_ = func; }

  template <class ObserverClass>
  Connection setOnKeyPressed(
      ObserverClass* observer,
      void (ObserverClass::*callback)(const KeyEventArgs&)) {
    return observables_.key_pressed.addObserver(observer, callback);
  }

  template <class ObserverClass>
  Connection setOnKeyReleased(
      ObserverClass* observer,
      void (ObserverClass::*callback)(const KeyEventArgs&)) {
    return observables_.key_released.addObserver(observer, callback);
  }

  template <class ObserverClass>
  Connection setOnMousePressed(
      ObserverClass* observer,
      void (ObserverClass::*callback)(const MouseEventArgs&)) {
    return observables_.mouse_pressed.addObserver(observer, callback);
  }

  template <class ObserverClass>
  Connection setOnMouseMoved(
      ObserverClass* observer,
      void (ObserverClass::*callback)(const MouseEventArgs&)) {
    return observables_.mouse_moved.addObserver(observer, callback);
  }

  template <class ObserverClass>
  Connection setOnMouseReleased(
      ObserverClass* observer,
      void (ObserverClass::*callback)(const MouseEventArgs&)) {
    return observables_.mouse_released.addObserver(observer, callback);
  }

  template <class ObserverClass>
  Connection setOnMouseScrolled(
      ObserverClass* observer,
      void (ObserverClass::*callback)(const ScrollEventArgs&)) {
    return observables_.mouse_scrolled.addObserver(observer, callback);
  }

  template <class ObserverClass>
  Connection setOnWindowResized(
      ObserverClass* observer,
      void (ObserverClass::*callback)(const ResizeEventArgs&)) {
    return observables_.window_resized.addObserver(observer, callback);
  }

#pragma mark LOOP BEGIN
  template <class ObserverClass>
  Connection setOnLoopBegin(ObserverClass* observer,
                            void (ObserverClass::*callback)(const EventArgs&)) {
    return observables_.loop_begin.addObserver(observer, callback);
  }

  template <typename Callback>
  Connection setOnLoopBegin(Callback callback) {
    std::function<void(const EventArgs&)> func = callback;
    return observables_.loop_begin.addObserver(callback);
  }

#pragma mark LOOP END
  template <class ObserverClass>
  Connection setOnLoopEnd(ObserverClass* observer,
                          void (ObserverClass::*callback)(const EventArgs&)) {
    return observables_.loop_end.addObserver(observer, callback);
  }

  template <typename Callback>
  Connection setOnLoopEnd(Callback callback) {
    std::function<void(const EventArgs&)> func = callback;
    return observables_.loop_end.addObserver(callback);
  }

  template <class ObserverClass>
  Connection setOnWindowRefreshed(
      ObserverClass* observer,
      void (ObserverClass::*callback)(const EventArgs&)) {
    return observables_.window_refreshed.addObserver(observer, callback);
  }

  template <class ObserverClass>
  Connection setOnWindowClosed(
      ObserverClass* observer,
      void (ObserverClass::*callback)(const EventArgs&)) {
    return observables_.window_closed.addObserver(observer, callback);
  }

  template <class ObserverClass>
  Connection setOnFileDropped(
      ObserverClass* observer,
      void (ObserverClass::*callback)(const FileDropEventArgs&)) {
    return observables_.file_dropped.addObserver(observer, callback);
  }

  GLFWwindow* getHandle() { return window_; };

 protected:
  uint32_t index_;
  GLFWwindow* window_;
  std::function<void(void)> draw_call_;
  const bool b_doublebuffering_;
  CoreObservable observables_;

  struct State {
    glm::vec2 pos;
    glm::vec2 size;
    bool b_fullscreen;
  };
  State state_;

 private:
  Window() : b_doublebuffering_(true) {}

  static void keyCallback(GLFWwindow* win, int key, int scancode, int action,
                          int mods) {
    int ascii = key;
    auto it = gl::glfwKeyToAsciiMap.find(key);
    if (it != gl::glfwKeyToAsciiMap.end()) ascii = it->second;

    KeyEventArgs e{ascii};
    Window* app = static_cast<Window*>(glfwGetWindowUserPointer(win));
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
      app->observables_.key_pressed.notify(e);
    else if (action == GLFW_RELEASE)
      app->observables_.key_released.notify(e);
  }

  static void mouseButtonCallback(GLFWwindow* win, int button, int action,
                                  int mods) {
    Window* app = static_cast<Window*>(glfwGetWindowUserPointer(win));
    double x, y;
    glfwGetCursorPos(win, &x, &y);
    MouseEventArgs e{x, y, button};
    if (action == GLFW_PRESS)
      app->observables_.mouse_pressed.notify(e);
    else if (action == GLFW_RELEASE)
      app->observables_.mouse_released.notify(e);
  }

  static void cursorPosCallback(GLFWwindow* win, double x, double y) {
    Window* app = static_cast<Window*>(glfwGetWindowUserPointer(win));
    int button =
        glfwGetMouseButton(win, MOUSE_BUTTON_LEFT)
            ? MOUSE_BUTTON_LEFT
            : (glfwGetMouseButton(win, MOUSE_BUTTON_RIGHT) ? MOUSE_BUTTON_RIGHT
                                                           : MOUSE_BUTTON_NONE);
    MouseEventArgs e{x, y, button};
    app->observables_.mouse_moved.notify(e);
  }

  static void scrollCallback(GLFWwindow* win, double x, double y) {
    Window* app = static_cast<Window*>(glfwGetWindowUserPointer(win));
    ScrollEventArgs e{x, y};
    app->observables_.mouse_scrolled.notify(e);
  }

  static void frameBufferSizeCallback(GLFWwindow* win, int width, int height) {
    Window* app = static_cast<Window*>(glfwGetWindowUserPointer(win));
    ResizeEventArgs e{width, height};
    app->observables_.window_resized.notify(e);
  }

  static void windowRefreshCallback(GLFWwindow* win) {
    Window* app = static_cast<Window*>(glfwGetWindowUserPointer(win));
    EventArgs e;
    app->observables_.window_refreshed.notify(e);
  }

  static void windowCloseCallback(GLFWwindow* win) {
    Window* app = static_cast<Window*>(glfwGetWindowUserPointer(win));
    EventArgs e;
    app->observables_.window_closed.notify(e);
  }

  static void fileDropCallback(GLFWwindow* win, int count, const char** paths) {
    Window* app = static_cast<Window*>(glfwGetWindowUserPointer(win));
    FileDropEventArgs e;
    e.paths.resize(count);
    for (int i = 0; i < count; i++) e.paths[i] = paths[i];
    app->observables_.file_dropped.notify(e);
  }
};

}  // namespace limas
