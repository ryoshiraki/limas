#pragma once
#include "app/BaseApp.h"
#include "geom/MeshIO.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;
  geom::MeshIO mesh_io;
  gl::VboMesh mesh;
  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    auto meshes = geom::MeshIO::load(fs::getFrameworkPath() +
                                     "assets/obj/capsule/capsule.obj");
    if (meshes.size()) {
      mesh = meshes[0];
    }
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
        "USR:" + util::toString(getUseCpuTime(), 2, 6, '0') + " MS", 5, 25);
    gl::drawBitmapString(
        "SYS:" + util::toString(getSystemCpuTime(), 2, 6, '0') + " MS", 5, 35);
    gl::drawBitmapString(
        "WAL:" + util::toString(getWallTime(), 2, 6, '0') + " MS", 5, 45);
    gl::drawBitmapString(
        "CPU:" + util::toString(getCpuUsage() * 100.0, 2, 6, '0') + " PC", 5,
        55);
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
