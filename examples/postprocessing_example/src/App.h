#pragma once
#include "3d/FPSCamera.h"
#include "app/BaseApp.h"
#include "net/OscReceiver.h"
#include "pp/Blur.h"
#include "pp/Brcosa.h"
#include "pp/Dof.h"
#include "pp/Fxaa.h"
#include "pp/Gaussian.h"
#include "pp/PostProcessing.h"
#include "primitives/Sphere.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  PostProcessing pp;

  FPSCamera cam;
  gl::InstancedVboMesh mesh;
  net::OscReceiver receiver;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    mesh = prim::Sphere(5, 64, 32, false);
    mesh.allocateInstances(300);

    auto &translations = mesh.getTranslations();
    auto &scales = mesh.getScales();
    auto &rotations = mesh.getRotations();
    auto &colors = mesh.getInstanceColors();
    for (int i = 0; i < translations.size(); i++) {
      translations[i] = math::randInSphere() * 100.0f;
      scales[i] = glm::vec3(math::randFloat(0.25, 1));
      rotations[i] =
          glm::angleAxis(math::randFloat() * math::twopi(), math::randVec3());
      colors[i] = glm::vec4(math::randFloat(0, 1), math::randFloat(0, 1),
                            math::randFloat(0, 1), 1);
    }
    mesh.updateInstances();

    cam.setPosition({0, 0, 100});
    cam.lookAt({0, 0, 0}, {0, 1, 0});
    cam.setNearClip(0.1);
    cam.setFarClip(300);
    cam.setFov(30);
    cam.setDistanceToPivot(100);
    cam.enableKeyInput(getWindows()[0]);
    cam.enableMouseInput(getWindows()[0]);

    pp.setup(getWidth(), getHeight());
    // pp.addPass<pp::Fxaa>("Fxaa");
    // pp.addPass<pp::Gaussian>("Gaussian");
    // pp.addPass<pp::Blur9>("Blur");
    // pp.addPass<pp::Brcosa>("Brcosa");
    pp.addPass<pp::Dof>("Dof")
        ->setParam<float>("cam", "near", cam.getNearClip())
        ->setParam<float>("cam", "far", cam.getFarClip());

    receiver.setup(7997);
    receiver.listen("/focus/length", [=](float v) {
      pp.getPass("Dof")->setParam<float>("focus", "length", v);
    });
    receiver.listen("/focus/depth", [=](float v) {
      pp.getPass("Dof")->setParam<float>("focus", "depth", v);
    });
    receiver.listen("/focus/fstop", [=](float v) {
      pp.getPass("Dof")->setParam<float>("focus", "fstop", v);
    });
    receiver.listen("/focus/coc", [=](float v) {
      pp.getPass("Dof")->setParam<float>("focus", "coc", v);
    });

    receiver.listen("/depth_blur/enable", [=](int v) {
      pp.getPass("Dof")->setParam<bool>("depth_blur", "enable", v);
    });
    receiver.listen("/depth_blur/size", [=](float v) {
      pp.getPass("Dof")->setParam<float>("depth_blur", "size", v);
    });
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);
    // gl::enableBlend();
    // gl::setBlendModeAlpha();

    pp.bind();
    gl::enableDepth();
    gl::clearColor(0, 0, 0, 0);
    gl::clearDepth();

    gl::pushMatrix();
    auto view = cam.getModelViewMatrix();
    auto proj = cam.getProjectionMatrix();
    gl::loadIdentity();
    gl::multMatrix(proj * view);
    gl::drawMeshInstanced(mesh, GL_TRIANGLES, mesh.getNumInstances());
    gl::popMatrix();

    // gl::pushMatrix();
    // gl::setOrthoView(getWidth(), getHeight());
    // gl::drawSegment(0, 0, 0, getWidth(), getHeight(), 0);

    // gl::drawBitmapString("FPS:" + util::toString(getFPS(), 2, 6, '0'), 5, 5);
    // gl::drawBitmapString(
    //     "MEM:" + util::toString(getMemoryUsage(), 2, 6, '0') + " MB", 5, 15);
    // gl::popMatrix();

    gl::disableDepth();
    pp.unbind();

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    gl::setColor(1, 1, 1, 1);
    gl::drawTexture(pp.getTexture(), 0, 0);
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
