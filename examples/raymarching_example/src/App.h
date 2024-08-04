#pragma once
#include "3d/FPSCamera.h"
#include "app/BaseApp.h"
#include "utils/HotReloader.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  FPSCamera cam;
  gl::Texture3D::Ptr texture;
  gl::Shader::Ptr shader;
  HotReloader reloader;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    cam.setPosition({0, 0, 100});
    cam.lookAt({0, 0, 0}, {0, 1, 0});
    cam.setNearClip(50);
    cam.setFarClip(10000);
    cam.setFov(45);
    cam.setDistanceToPivot(100);
    cam.enableInput(getWindows()[0]);

    shader =
        gl::Shader::create(fs::getAssetsPath() + "shader/raymarching.vert",
                           fs::getAssetsPath() + "shader/raymarching.frag");
    reloader.watchShader(*shader);
  }

  void draw() {
    gl::clearColor(0, 0, 0, 1);
    gl::clearDepth();

    // gl::enableDepth();
    // gl::pushMatrix();
    // auto view = cam.getModelViewMatrix();
    // auto proj = cam.getProjectionMatrix();
    // gl::loadIdentity();
    // gl::multMatrix(proj * view);
    // gl::setColor(1, 1, 1, 1);
    // gl::drawAxis(1000);
    // gl::popMatrix();

    gl::disableDepth();
    gl::pushMatrix();
    gl::setShader(*shader);
    shader->bind();
    shader->setUniform2f("u_resolution", getWindowSize());
    shader->setUniform2f("u_mouse", getMousePos());
    gl::fillScreen();
    gl::popMatrix();

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    gl::setShaderDefault();
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
