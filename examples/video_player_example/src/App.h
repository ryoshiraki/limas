#pragma once
#include "app/BaseApp.h"
#include "video/VideoPlayer.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  VideoPlayer player;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);
  }

  void draw() {
    player.update();

    gl::clearColor(0, 0, 0, 0);

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    if (player.isLoaded()) {
      gl::setColor(1, 1, 1, 1);
      gl::drawTexture(player.getTexture(), 0, 0);
    }
    gl::setColor(1, 1, 1, 1);
    gl::drawBitmapString("FPS:" + util::toString(getFPS(), 2, 6, '0'), 5, 5);
    gl::drawBitmapString(
        "MEM:" + util::toString(getMemoryUsage(), 2, 6, '0') + " MB", 5, 15);
    gl::popMatrix();
  }

  void keyPressed(const rs::KeyEventArgs &e) {
    if (e.key == '0') player.seekPosition(0.0);
    if (e.key == '1') player.seekPosition(0.2);
    if (e.key == '2') player.seekPosition(0.4);
    if (e.key == '3') player.seekPosition(0.6);
    if (e.key == '4') player.seekPosition(0.8);

    if (e.key == '6') player.setSpeed(0.5);
    if (e.key == '7') player.setSpeed(1);
    if (e.key == '8') player.setSpeed(2);

    if (e.key == ' ') player.isPlaying() ? player.pause() : player.play();
  }

  void keyReleased(const rs::KeyEventArgs &e) {}

  void mouseMoved(const rs::MouseEventArgs &e) {}

  void mousePressed(const rs::MouseEventArgs &e) {}

  void mouseReleased(const rs::MouseEventArgs &e) {}

  void mouseScrolled(const rs::ScrollEventArgs &e) {}

  void windowResized(const rs::ResizeEventArgs &e) {}

  void windowRefreshed(const rs::EventArgs &e) {}

  void windowClosed(const rs::EventArgs &e) {}

  void fileDropped(const rs::FileDropEventArgs &e) {
    if (player.load(e.paths[0])) {
      player.play();
    } else {
      log::error("App") << "failed to load " << e.paths[0] << log::end();
    }
  }
};

}  // namespace rs
