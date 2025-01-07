#pragma once
#include "app/BaseApp.h"
#include "video/VideoExporter.h"

namespace limas {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  VideoExporter exporter_;

  void setup() {
    setFPS(30);
    exporter_.allocate(getWidth(), getHeight(), 30);
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    exporter_.bind();
    gl::clearColor(0, 0, 0, 1);
    gl::pushMatrix();
    gl::setOrthoView(exporter_.getWidth(), exporter_.getHeight());
    gl::popMatrix();
    exporter_.unbind();

    gl::setColor(1, 1, 1, 1);
    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    gl::drawTexture(exporter_.getTexture(), 0, 0, getWidth(), getHeight(),
                    true);
    gl::drawBitmapString("FPS:" + utils::toString(getFPS(), 2, 6, '0'), 5, 5);
    gl::drawBitmapString(
        "MEM:" + utils::toString(getMemoryUsage(), 2, 6, '0') + " MB", 5, 15);
    gl::popMatrix();
  }

  void keyPressed(const KeyEventArgs &e) {
    if (e.key == 'r') {
      exporter_.start(fs::getAssetPath() + util::getTimestamp() + ".mov");
    } else if (e.key == 'q') {
      exporter_.stop();
      exit();
    }
  }

  void keyReleased(const KeyEventArgs &e) {}

  void mouseMoved(const MouseEventArgs &e) {}

  void mousePressed(const MouseEventArgs &e) {}

  void mouseReleased(const MouseEventArgs &e) {}

  void mouseScrolled(const ScrollEventArgs &e) {}

  void windowResized(const ResizeEventArgs &e) {}

  void windowRefreshed(const EventArgs &e) {}

  void windowClosed(const EventArgs &e) {}

  void fileDropped(const FileDropEventArgs &e) {}
};

}  // namespace limas
