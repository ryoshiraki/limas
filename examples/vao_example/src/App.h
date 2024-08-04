#pragma once
#include "3d/FPSCamera.h"
#include "app/BaseApp.h"
#include "graphics/ImageIO.h"

namespace rs {

using namespace std;

class CustomPoint {
 public:
  CustomPoint() {}
  CustomPoint(const glm::vec3 &vertex,
              const Color &color) {  // const glm::vec4 &color) {
    vertex_ = vertex;
    color_ = color;
  };
  glm::vec3 vertex_;
  Color color_;  // glm::vec4 color_;
};

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;
  gl::Vao::Ptr vao;
  gl::Vbo<CustomPoint>::Ptr vbo;
  vector<CustomPoint> points;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    float radius = math::randFloat(100, 200);
    points.resize(200);
    for (int i = 0; i < points.size(); i++) {
      float t = (float)i / points.size();
      float rad = t * math::twopi();
      float x = getWidth() * 0.5 + radius * cos(rad);
      float y = getHeight() * 0.5 + radius * sin(rad);
      points[i] = CustomPoint{
          glm::vec3{x, y, 0},
          Color{cosf(rad) * 0.5f + 0.5f, sinf(rad) + 0.5f + 0.5f, 1, 1}};
    }

    vao = gl::Vao::create();
    vbo = gl::Vbo<CustomPoint>::create();
    vbo->setData(points, GL_DYNAMIC_DRAW);
    vao->bindVbo(*vbo, POSITION_ATTRIBUTE, 3, GL_FLOAT, 0);
    vao->bindVbo(*vbo, COLOR_ATTRIBUTE, 4, GL_FLOAT,
                 (void *)(sizeof(glm::vec3)));
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);
    gl::clearDepth();

    gl::disableDepth();
    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    gl::setColor(1, 1, 1, 1);
    gl::bindShader();
    gl::drawArrays(*vao, GL_LINE_LOOP, points.size());
    gl::unbindShader();

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
    if (e.key == ' ') {
      float radius = math::randFloat(100, 300);
      for (int i = 0; i < points.size(); i++) {
        float t = (float)i / points.size();
        float rad = t * math::twopi();
        float x = getWidth() * 0.5 + radius * cos(rad);
        float y = getHeight() * 0.5 + radius * sin(rad);
        points[i] = CustomPoint{
            glm::vec3{x, y, 0},
            Color{cosf(rad) * 0.5f + 0.5f, sinf(rad) + 0.5f + 0.5f, 1, 1}};
      }
      vbo->update(points, 0);
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
