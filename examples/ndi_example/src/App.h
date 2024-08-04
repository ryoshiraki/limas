#pragma once
#include "NDIReceiver.h"
#include "NDISender.h"
#include "app/BaseApp.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  NDISender sender;
  NDIReceiver receiver;

  void setup() {
    setFPS(30.0f);
    setVerticalSync(true);

    sender.setup("example", 640, 480);
    sender.enableAsync();
    if (!receiver.connect("example", "127.0.0.1")) {
      log::error() << "failed to connect" << log::end();
    }
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    sender.bind();
    gl::clearColor(1, 0, 0, 1);
    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    float radius = fmod(getElapsedTimeInSeconds(), 10.0f) / 10.0f;
    radius = 50 + radius * 200;
    gl::setColor(1, 1, 1, 1);
    gl::drawCircle(getWidth() * 0.5, getHeight() * 0.5, radius);

    gl::setColor(1, 1, 1, 1);
    gl::drawBitmapString("FPS:" + util::toString(getFPS(), 2, 6, '0'), 5, 5);
    gl::drawBitmapString(
        "MEM:" + util::toString(getMemoryUsage(), 2, 6, '0') + " MB", 5, 15);
    gl::drawBitmapString(
        "USR:" + util::toString(getUseCpuTime(), 2, 6, '0') + " MS", 5, 25);
    gl::drawBitmapString(
        "SYS:" + util::toString(getSystemCpuTime(), 2, 6, '0') + " MS", 5, 35);
    gl::drawBitmapString(
        "WAL:" + util::toString(getWallTime(), 2, 6, '0') + " MS", 5, 45);
    gl::drawBitmapString(
        "CPU:" + util::toString(getCpuUsage() * 100.0, 2, 6, '0') + " PC", 5,
        55);
    gl::popMatrix();
    sender.unbind();

    receiver.update();
    if (receiver.isFrameNew()) {
      gl::pushMatrix();
      gl::setOrthoView(getWidth(), getHeight());
      gl::flip(false, true);
      gl::setColor(1, 1, 1, 1);
      gl::drawTexture(*(receiver.getTexture()), 0, 0);
      gl::popMatrix();
    }
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
