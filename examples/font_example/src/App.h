#pragma once
#include "3d/FPSCamera.h"
#include "app/BaseApp.h"
#include "graphics/Font.h"
#include "graphics/ImageIO.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;
  Font font;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    font.load(fs::getResourcesPath() + "fonts/SanFranciscoDisplay-Regular.otf",
              48, true);
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);
    gl::clearDepth();

    gl::disableDepth();
    gl::enableBlend();
    gl::setBlendModeAlpha();

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    auto &font_tex = font.getTexture();
    int x = getFrameNumber() % font_tex->getWidth();
    gl::setColor(1, 1, 1, 1);
    gl::drawTexture(*font_tex, -x, 0);

    std::string text = "HELLO,WORLD!?1234asdf";
    gl::VboMesh text_mesh = font.getMesh(text, 0, getHeight() * 0.5);
    gl::VboMesh text_bbox = font.getBoundingBox(text, 0, getHeight() * 0.5);

    gl::bindTexture(*font_tex);
    gl::setColor(1, 1, 1, 1);
    gl::drawMesh(text_mesh, GL_TRIANGLES);
    gl::unbindTexture();
    gl::setColor(1, 1, 1, 0.5);
    gl::drawMesh(text_bbox, GL_LINE_LOOP);
    gl::drawSegment(0, getHeight() * 0.5, 0, getWidth(), getHeight() * 0.5, 0);

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

    gl::setColor(1, 1, 1, 0.25);
    gl::drawGrid(getWidth(), getHeight(), 60, 60);

    gl::popMatrix();
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
