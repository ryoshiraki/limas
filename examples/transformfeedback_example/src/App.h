#pragma once
#include "app/BaseApp.h"
#include "gl/TransformFeedback.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  gl::TransformFeedback::Ptr tf;
  gl::Shader::Ptr shader;
  gl::Vao::Ptr vao;
  gl::Vbo<glm::vec3>::Ptr vbo;

  std::vector<glm::vec3> data;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    data.resize(128);
    for (int i = 0; i < data.size(); i++) {
      float rad = ((float)i / (float)data.size()) * math::pi() * 2.0f;
      float x = 200 * cos(rad);
      float y = 200 * sin(rad);
      data[i] = glm::vec3(x, y, 0);
    }
    vao = gl::Vao::create();
    vbo = gl::Vbo<glm::vec3>::create();
    vbo->allocate(data, GL_DYNAMIC_COPY);
    vao->bindVbo(*vbo, POSITION_ATTRIBUTE, 3, GL_FLOAT, 0);

    shader = gl::Shader::create();
    shader->loadVertex(fs::getAssetsPath() + "shader/tf.vert");
    vector<string> varyings = {"v_position"};
    shader->setTransformFeedbackVaryings(varyings);
    shader->link();
    gl::checkError("Shader");

    tf = gl::TransformFeedback::create();
    tf->bindBuffer(0, vbo->getID());
    gl::checkError("TransformFeedback");
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    shader->bind();
    tf->begin(GL_POINTS);
    vao->draw(GL_POINTS, 0, vbo->getSize());
    tf->end();
    shader->unbind();

    // vbo->copyDataTo(&data, 0, vbo->getSize());
    // cout << data[0].x << ", " << data[0].y << ", " << data[0].z << endl;

    const glm::vec3 *ptr = vbo->getDataPointer(0, vbo->getSize());
    if (ptr != nullptr) {
      log::info() << glm::length(ptr[0]) << log::end();
    } else {
      log::error("App") << "Couldn't fetch data" << log::end();
    }

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    gl::setColor(1, 1, 1, 1);
    gl::drawArrays(*vao, GL_LINE_LOOP, vbo->getSize());
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