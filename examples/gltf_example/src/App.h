#pragma once
#include "3d/FPSCamera.h"
#include "BaseApp.h"
#include "geom/Primitives3d.h"
#include "graphics/Image.h"
#include "utils/Stats.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;
  Image image;
  FPSCamera cam;
  gl::VboMesh mesh;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    if (!image.load(fs::getFrameworkPath() + "resources/cat.jpg")) {
      log::warn() << "cannot load image" << log::end();
    } else {
      image.getTexture().setMinFilter(GL_LINEAR);
      image.getTexture().setMagFilter(GL_LINEAR);
      image.getTexture().setWrapS(GL_REPEAT);
      image.getTexture().setWrapT(GL_REPEAT);
    }

    cam.setPosition({0, 0, 100});
    cam.lookAt({0, 0, 0}, {0, 1, 0});
    cam.setNearClip(1e-2);
    cam.setFarClip(INT_MAX);
    cam.setFov(45);
    cam.setDistanceToPivot(100);
    cam.enableInput(getWindows()[0]);

    mesh = prim::IcoSphere(100, 1, true).toMesh();

    log::info() << std::left << std::setw(12)
                << "Project:" << fs::getProjectPath() << log::endl;
    log::info() << std::left << std::setw(12)
                << "Executable:" << fs::getExecutablePath() << log::endl;
    log::info() << std::left << std::setw(12)
                << "Assets:" << fs::getAssetsPath() << log::endl;
    log::info() << std::left << std::setw(12)
                << "Framework:" << fs::getFrameworkPath() << log::endl;
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);
    gl::clearDepth();

    gl::disableDepth();
    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    gl::setColor(1, 1, 1, 1);
    gl::drawTextureSubsection(image.getTexture(), 0, 0, getWidth(), getHeight(),
                              0, 0, getWidth(), getHeight());

    int y = fmod(getFrameNumber(), getHeight());
    gl::setColor(1, 1, 1, 1);
    gl::drawSegment(0, y, 0, getWidth(), y, 0);

    gl::setColor(1, 1, 1, 1);
    gl::drawBitmapString("FPS:" + util::toString(getFPS(), 3, 0, '0'), 5, 5);
    gl::drawBitmapString(
        "MEM:" + util::toString(util::getMemoryUsage(), 3, 0, '0'), 5, 15);
    gl::drawBitmapString(
        "CPU:" + util::toString(util::getCpuUsage() * 100.0, 3, 0, '0'), 5, 25);
    gl::popMatrix();

    gl::enableDepth();
    gl::pushMatrix();
    auto view = cam.getModelViewMatrix();
    auto proj = cam.getProjectionMatrix();
    gl::loadIdentity();
    gl::multMatrix(proj * view);
    gl::drawMesh(mesh, GL_LINES);
    gl::drawAxis(1000);
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
