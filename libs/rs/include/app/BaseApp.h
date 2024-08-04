#pragma once
#include "app/Draw.h"
#include "app/Window.h"
#include "math/Math.h"
#include "system/Logger.h"
#include "utils/FileSystem.h"
#include "utils/Stats.h"

namespace rs {

class BaseApp {
 public:
  using Ptr = std::shared_ptr<BaseApp>;

  BaseApp(const Window::Settings& settings) {
    glfwSetErrorCallback(BaseApp::errorCallback);
    if (!glfwInit()) {
      log::error("BaseApp") << "failed to initialize GLFW" << log::end();
      glfwTerminate();
    }

    main_window_ = Window::createWindow(settings, nullptr);
    if (main_window_ == nullptr) {
      log::error("BaseApp") << "failed to create window" << log::end();
      glfwTerminate();
    }
    windows_.push_back(main_window_);

    main_window_->bind();
    main_window_->setDrawCall(this, &BaseApp::draw);
    main_window_->setOnKeyPressed(this, &BaseApp::keyPressed);
    main_window_->setOnKeyReleased(this, &BaseApp::keyReleased);
    main_window_->setOnMouseMoved(this, &BaseApp::mouseMoved);
    main_window_->setOnMousePressed(this, &BaseApp::mousePressed);
    main_window_->setOnMouseReleased(this, &BaseApp::mouseReleased);
    main_window_->setOnMouseScrolled(this, &BaseApp::mouseScrolled);
    main_window_->setOnWindowResized(this, &BaseApp::windowResized);
    main_window_->setOnWindowRefreshed(this, &BaseApp::windowRefreshed);
    main_window_->setOnWindowClosed(this, &BaseApp::windowClosed);
    main_window_->setOnFileDropped(this, &BaseApp::fileDropped);
    current_window_ = main_window_;

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
      log::error("BaseApp") << "failed to initialize GLEW" << log::end();
      glfwTerminate();
    }
  }

  Window::Ptr addWindow(const Window::Settings& settings) {
    auto window = Window::createWindow(settings, main_window_->getHandle());
    windows_.push_back(window);
    return window;
  }

  void run() {
    log::info("BaseApp") << "start running" << log::end();

    if (windows_.empty()) {
      log::error("BaseApp") << "no window found" << log::end();
      glfwTerminate();
      return;
    }

    glfwSwapInterval(1);
    glfwSetTime(0);

    for_each(begin(windows_), end(windows_), [&](Window::Ptr& w) {
      w->bind();
      glClear(GL_COLOR_BUFFER_BIT);
      w->unbind();
    });

    renderer_ = gl::Renderer::create();
    setup();

    elapsed_seconds_ = 0;
    frame_number = 0;
    double last_frame_time = 0;
    while (std::all_of(begin(windows_), end(windows_), [](Window::Ptr& w) {
      return !glfwWindowShouldClose(w->getHandle());
    })) {
      double now = glfwGetTime();
      glfwPollEvents();

      if ((now - last_frame_time) >= (1.0 / target_fps_)) {
        stats_.begin();
        for_each(begin(windows_), end(windows_), [&](Window::Ptr& w) {
          current_window_ = w;
          w->bind();
          w->draw();
          w->unbind();
        });
        stats_.end();

        frame_number++;
        current_fps_ = 1.0 / (now - last_frame_time);
        last_frame_time = now;
      }

      current_window_ = main_window_;
      elapsed_seconds_ = now;
    }

    glfwTerminate();
    log::info("App") << "stop running" << log::end();
  }

  std::vector<Window::Ptr>& getWindows() { return windows_; }
  Window::Ptr& getMainWindow() { return main_window_; }
  Window::Ptr& getCurrentWindow() { return current_window_; }

  void setVerticalSync(bool vsync) { glfwSwapInterval(vsync); }
  void setFPS(float fps) { this->target_fps_ = fps; }
  double getFPS() const { return current_fps_; }

  double getWallTime() const { return stats_.getWallTimeInMs(); }
  double getUseCpuTime() const { return stats_.getUserCpuTimeInMs(); }
  double getSystemCpuTime() const { return stats_.getSystemCpuTimeInMs(); }
  double getCpuTime() const { return stats_.getCpuTimeInMs(); }
  double getMemoryUsage() const { return stats_.getMemoryUsageInMb(); }
  double getCpuUsage() const { return stats_.getCpuUsageInPerc(); }

  double getElapsedTimeInSeconds() const { return elapsed_seconds_; }
  uint32_t getFrameNumber() const { return frame_number; }

  void setWindowTitle(const std::string& title) {
    current_window_->setTitle(title);
  }

  void setWindowPosition(int x, int y) { current_window_->setPosition(x, y); }
  glm::vec2 getWindowPosition() const { return current_window_->getPosition(); }

  void setWindowSize(int w, int h) { current_window_->setSize(w, h); }
  glm::vec2 getWindowSize() const { return current_window_->getSize(); }
  int getWidth() const { return current_window_->getWidth(); }
  int getHeight() const { return current_window_->getHeight(); }

  glm::vec2 getMousePos() const { return current_window_->getMousePos(); }
  float getMouseX() const { return current_window_->getMouseX(); }
  float getMouseY() const { return current_window_->getMouseY(); }

  void setFullScreen() { current_window_->setFullScreen(); }
  void setDefaultScreen() { current_window_->setDefaultScreen(); }
  void toggleFullscreen() { current_window_->toggleFullscreen(); }

  void exit() { std::exit(0); }

  virtual void setup() {}
  virtual void update() {}
  virtual void draw() {}

  virtual void keyPressed(const KeyEventArgs& e) {}
  virtual void keyReleased(const KeyEventArgs& e) {}
  virtual void mouseMoved(const MouseEventArgs& m) {}
  virtual void mousePressed(const MouseEventArgs& m) {}
  virtual void mouseReleased(const MouseEventArgs& m) {}
  virtual void mouseScrolled(const ScrollEventArgs& m) {}
  virtual void windowResized(const ResizeEventArgs& m) {}
  virtual void windowRefreshed(const EventArgs& e) {}
  virtual void windowClosed(const EventArgs& e) {}
  virtual void fileDropped(const FileDropEventArgs& e) {}

  gl::Renderer::Ptr& getRenderer() { return renderer_; }
  gl::Context* getContext() { return &context_; }

 private:
  gl::Renderer::Ptr renderer_;
  gl::Context context_;
  std::vector<Window::Ptr> windows_;
  Window::Ptr main_window_;
  Window::Ptr current_window_;
  Stats stats_;

  double target_fps_ = 60;
  float current_fps_ = 60;
  double elapsed_seconds_ = 0;
  uint32_t frame_number = 0;

  static void errorCallback(int code, const char* description) {
    log::error("BaseApp") << code << ": " << description << log::end();
  }
};

}  // namespace rs
