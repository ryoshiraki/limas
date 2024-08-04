#pragma once
#include <numeric>

#include "3d/FPSCamera.h"
#include "app/BaseApp.h"
#include "graphics/ImageIO.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);
    gl::clearDepth();

    gl::disableDepth();
    gl::disableBlend();
    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    auto common_divisors = math::getCommonDivisors({getWidth(), getHeight()});
    int index =
        math::lerp(getMouseX(), 0, getWidth(), 0, common_divisors.size(), true);
    int step = common_divisors[index];
    gl::setColor(1, 1, 1, 1);
    gl::drawGrid(getWidth(), getHeight(), step, step);

    gl::popMatrix();
  }

  void keyPressed(const rs::KeyEventArgs &e) {
    if (e.key == 'f') getMainWindow()->toggleFullscreen();
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
