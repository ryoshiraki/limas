#pragma once
#include "3d/FPSCamera.h"
#include "app/BaseApp.h"
#include "graphics/ImageIO.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;
  gl::VboPolyline poly;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    float radius = math::randFloat(100, 200);
    int n = 200;
    for (int i = 0; i < n; i++) {
      float t = (float)i / n;
      float rad = t * math::twopi();
      float x = getWidth() * 0.5 + radius * cos(rad);
      float y = getHeight() * 0.5 + radius * sin(rad);
      poly.addVertex(glm::vec3{x, y, 0});
    }

    poly = poly.getResampledByCount(n / 10);

    poly.update();
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);
    gl::clearDepth();

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    gl::setColor(1, 1, 1, 1);
    gl::drawPolyline(poly, GL_LINE_LOOP);

    gl::popMatrix();
  }

  void keyPressed(const rs::KeyEventArgs &e) {
    if (e.key == ' ') {
      auto &vertices = poly.getVertices();
      float radius = math::randFloat(100, 300);
      for (int i = 0; i < vertices.size(); i++) {
        float t = (float)i / vertices.size();
        float rad = t * math::twopi();
        float x = getWidth() * 0.5 + radius * cos(rad);
        float y = getHeight() * 0.5 + radius * sin(rad);
        vertices[i] = glm::vec3{x, y, 0};
      }
      poly.update();
    }
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
