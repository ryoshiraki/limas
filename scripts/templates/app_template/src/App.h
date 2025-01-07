#pragma once
#include "app/BaseApp.h"

namespace limas {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  void setup() {
    setVerticalSync(true);

    logger::info() << std::left << std::setw(12)
                   << "Project:" << fs::getProjectPath() << logger::end();
    logger::info() << std::left << std::setw(12)
                   << "Assets:" << fs::getAssetPath() << logger::end();
    logger::info() << std::left << std::setw(12)
                   << "Resources:" << fs::getResourcePath() << logger::end();
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    gl::setColor(1, 1, 1, 1);
    gl::drawBitmapString("FPS:" + utils::toString(getFPS(), 2, 6, '0'), 5, 5);
    gl::drawBitmapString(
        "MEM:" + utils::toString(getMemoryUsage(), 2, 6, '0') + " MB", 5, 15);
    gl::popMatrix();
  }

  void keyPressed(const KeyEventArgs &e) {
    if (e.key == 'f') getMainWindow()->toggleFullscreen();
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
