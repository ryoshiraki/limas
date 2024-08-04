#pragma once
#include "app/BaseApp.h"
#include "math/Oscillator.h"

namespace rs {

using namespace std;

class Scope {
 public:
  Scope() : index_(0) {
    amps_.resize(180);
    vbo_.allocate(amps_.data(), amps_.size(), GL_DYNAMIC_DRAW);

    std::vector<int> indices(180);
    for (int i = 0; i < indices.size(); i++) indices[i] = i;
    ibo_.allocate(indices.data(), indices.size());

    vao_.bindVbo(vbo_, 0, 1);
    vao_.bindIbo(ibo_);

    shader_.load(fs::getAssetsPath("shader/osc.vert"),
                 fs::getAssetsPath("shader/osc.frag"));
  }
  ~Scope() {}

  void resize(int size) { amps_.resize(size); }

  void add(float a) {
    amps_[index_] = a;
    index_++;
    index_ %= amps_.size();
    vbo_.update(amps_, 0);
  }

  void draw() {
    shader_.bind();
    shader_.setUniform1i("u_size", amps_.size());
    vao_.drawElements(GL_LINE_STRIP, amps_.size());
    shader_.unbind();
  }

 private:
  int index_;
  gl::Vao vao_;
  gl::Vbo<float> vbo_;
  gl::Ibo<int> ibo_;
  gl::Shader shader_;
  std::vector<float> amps_;
};

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  Scope scope;

  math::SineWave sine;
  math::SineWave f_sine;
  math::SineWave a_sine;

  gl::Shader shader;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    sine.setFrequency(0.5);
    sine.setAmplitude(0.3);
    sine.setPhase(math::randFloat() * math::twopi());

    f_sine.setFrequency(0.4);
    f_sine.setAmplitude(0.3);
    f_sine.setPhase(math::randFloat() * math::twopi());

    a_sine.setFrequency(0.3);
    a_sine.setAmplitude(0.3);
    a_sine.setPhase(math::randFloat() * math::twopi());
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);
    float sec = getElapsedTimeInSeconds() + 200.0;
    sine.setFrequency(f_sine.get(sec) + 1);

    float a = sine.get(sec);
    a += f_sine.get(sec);
    a += a_sine.get(sec);

    scope.add(a);
    scope.draw();

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    gl::setColor(1, 1, 1, 1);
    gl::drawBitmapString("FPS:" + util::toString(getFPS(), 2, 6, '0'), 5, 5);
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
