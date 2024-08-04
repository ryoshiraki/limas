#pragma once
#include "app/BaseApp.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  void setup() {
    setVerticalSync(true);

    log::info() << std::left << std::setw(12)
                << "Project:" << fs::getProjectPath() << log::endl;
    log::info() << std::left << std::setw(12)
                << "Executable:" << fs::getExecutablePath() << log::endl;
    log::info() << std::left << std::setw(12)
                << "Assets:" << fs::getAssetsPath() << log::endl;
    log::info() << std::left << std::setw(12)
                << "Framework:" << fs::getFrameworkPath() << log::endl;
    log::info() << std::left << std::setw(12)
                << "Resources:" << fs::getResourcesPath() << log::endl;
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    gl::setColor(1, 1, 1, 1);
    gl::drawBitmapString("FPS:" + util::toString(getFPS(), 2, 6, '0'), 5, 5);
    gl::drawBitmapString(
        "MEM:" + util::toString(getMemoryUsage(), 2, 6, '0') + " MB", 5, 15);
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
