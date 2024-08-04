#pragma once
#include "app/BaseApp.h"
#include "cv/Cv.h"
#include "py/Py.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  Py py;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    py.load(fs::getAssetsPath() + "python/test.py");
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);
    gl::clearDepth();

    gl::disableDepth();
    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    Image image;
    cv::Mat src, blur, gray, canny;

    Pixels2D pixels = py.call("test").convert<NdArray>().convert<Pixels2D>();

    image.setFromPixes(pixels);
    gl::drawTexture(*image.getTexture(), 0, 0);

    src = toCv(pixels);
    cv::blur(src, blur, cv::Size(7, 7));

    toPixels(blur, pixels);
    image.setFromPixes(pixels);
    gl::drawTexture(*image.getTexture(), 0, pixels.getHeight());

    cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);
    toPixels(gray, pixels);
    image.setFromPixes(pixels);
    gl::drawTexture(*image.getTexture(), pixels.getWidth(), 0);

    cv::Canny(src, canny, 50, 100);
    toPixels(canny, pixels);
    image.setFromPixes(pixels);
    gl::drawTexture(*image.getTexture(), pixels.getWidth(), pixels.getHeight());

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
