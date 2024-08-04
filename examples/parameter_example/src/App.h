#pragma once
#include "app/BaseApp.h"
#include "type/ParameterGroup.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  Parameter<int> param;
  ParameterGroup group;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    param.setName("d");
    param.setValue(1000);
    param.setMin(0);
    param.setMax(10000);
    log::info("a") << param << log::end();

    group.setName("test");
    group.addParam<int>("a", 0);
    group.addParam<float>("b", 3.14);
    group.addParam<float>("c", 0.5, 0, 1);
    group.setValue<float>("c", 2);
    group.addParam(param);
    group.addParam<std::string>("e", "hello");
    group.addParam<glm::vec3>("f", glm::vec3(-10, 0, 10), glm::vec3(-5),
                              glm::vec3(5));

    param.setValue(2000);

    group.addChild("aaa").addParam<float>("g", 1);
    group.addChild("bbb").addParam<float>("h", 2);
    group.getChild("aaa").addChild("ccc").addParam<float>("i", 3);

    try {
      log::info("a") << group.getValue<int>("a") << log::end();
      log::info("b") << group.getValue<float>("b") << log::end();
      log::info("c") << group.getValue<float>("c") << log::end();
      log::info("d") << group.getValue<int>("d") << log::end();
      log::info("e") << group.getValue<std::string>("e") << log::end();
      log::info("f") << group.getValue<glm::vec3>("f") << log::end();
      log::info("g") << group.getChild("child").getValue<float>("g")
                     << log::end();

    } catch (rs::Exception &e) {
      log::error("test") << e.what() << log::end();
    }

    log::info("a") << group << log::end();
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
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
