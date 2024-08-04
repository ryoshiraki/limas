#pragma once
#include "app/BaseApp.h"
#include "system/Thread.h"

namespace rs {

class Test : public Thread {
 public:
  void play() {
    auto locker = getLock();
    b_playing_ = true;
    notify();
  }

  void pause() {
    auto locker = getLock();
    b_playing_ = false;
    notify();
  }

 private:
  bool b_playing_ = true;
  void threadedFunction() {
    while (isThreadRunning()) {
      auto locker = getLock();

      waitFor(locker, [this] { return b_playing_; });

      log::info("thread") << app::getElapsedTimeInSeconds() << log::end();

      // sleepFor(100);
    }
  }
};

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  Test test;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    test.startThread();
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    gl::setColor(1, 1, 1, 1);
    gl::drawBitmapString("FPS:" + util::toString(getFPS(), 2, 6, '0'), 5, 5);
    gl::drawBitmapString(
        "MEM:" + util::toString(getMemoryUsage(), 2, 6, '0') + " MB", 5, 15);
    gl::drawBitmapString(
        "TME:" + util::toString(getElapsedTimeInSeconds(), 2, 6, '0'), 5, 25);

    gl::popMatrix();
  }

  void keyPressed(const rs::KeyEventArgs &e) {
    if (e.key == 'z') test.play();
    if (e.key == 'x') test.pause();
  }

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
