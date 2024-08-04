#pragma once
#include "app/BaseApp.h"
#include "utils/Timer.h"
#include "utils/Tween.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;
  PreciseTimer timer;
  PreciseTimer::Node::Ptr node;
  Tween tween;
  Tween::Node<float>::Ptr tween_node;

  void onEnd() { log::info() << "done" << log::end(); }

  void setup() {
    setFPS(30.0f);
    setVerticalSync(true);

    node = timer.add("test", std::chrono::seconds(4), true,
                     std::bind(&App::onEnd, this));
    timer.start();

    tween.add<float>("r", 0, 1, std::chrono::seconds(5), true,
                     &math::easing::quadIn);
    tween_node = tween.add<float>("g", 0, 1, std::chrono::seconds(10), true,
                                  &math::easing::quadIn);
    tween.start();
  }

  void draw() {
    timer.update();
    tween.update();

    float r = tween.getValue<float>("r");
    float g = tween_node->getValue();
    gl::clearColor(r, g, 0, 1);
  }

  void keyPressed(const rs::KeyEventArgs &e) {}

  void keyReleased(const rs::KeyEventArgs &e) {}

  void mouseMoved(const rs::MouseEventArgs &e) {}

  void mousePressed(const rs::MouseEventArgs &e) {}

  void mouseReleased(const rs::MouseEventArgs &e) {}

  void mouseScrolled(const rs::ScrollEventArgs &e) {}

  void windowResized(const rs::ResizeEventArgs &e) {}

  void windowRefreshed(const rs::EventArgs &e) {}

  void windowClosed(const rs::EventArgs &e) {}

  void fileDropped(const rs::FileDropEventArgs &e) {}
};

}  // namespace rs
