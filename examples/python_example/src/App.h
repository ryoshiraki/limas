#pragma once
#include "app/BaseApp.h"
#include "py/Py.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  Py py;
  gl::Texture2D::Ptr tex;
  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    Py::initialize();
    py.load(fs::getAssetsPath() + "python/test.py");

    py.call("hello");

    tex = gl::Texture2D::create(640, 360, GL_RGB8);
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);
    gl::clearDepth();

    gl::disableDepth();
    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    auto test_glm = py.call("test_glm").convert<glm::vec4>();
    auto test_tuple =
        py.call("test_tuple").convert<std::tuple<int, float, string>>();
    auto test_dict = py.call("test_dict").convert<std::map<string, int>>();

    auto test_vector2d =
        py.call("test_vector2d").convert<vector<vector<int>>>();

#if 0
    auto test_vector3d =
        py.call("test_vector3d").convert<vector<vector<vector<int>>>>();
    auto test_vector2d_glm =
        py.call("test_vector3d").convert<vector<vector<glm::vec2>>>();

    for (auto &l2 : test_vector3d) {
      for (auto &l : l2) {
        for (auto &v : l) {
          log::info("test_vector3d") << v << log::end();
        }
      }
    }

    for (auto &l2 : test_vector2d_glm) {
      for (auto &v : l2) {
        log::info("test_vector2d_glm") << v << log::end();
      }
    }
#endif

#if 0
    auto test_np_rand =
        py.call("test_np_rand").convert<NdArray>().convert<vector<float>>();
    for (auto n : test_np_rand) {
      log::info("test_np_rand") << n << log::end();
    }
#endif

#if 0
    auto arr = py.call("test_np_rand_2d").convert<NdArray>();
    auto np_rand_vec4 = arr.convert<vector<glm::vec4>>();
    auto np_rand_2d = arr.convert<vector<vector<float>>>();
    for (int i = 0; i < np_rand_vec4.size(); i++) {
      log::info("np_rand_vec4") << np_rand_vec4[i] << log::end();
    }
#endif

#if 1
    Pixels2D pixels =
        py.call("test_cv2").convert<NdArray>().convert<Pixels2D>();

    Image image;
    image.setFromPixes(pixels);
    gl::drawTexture(*image.getTexture(), 0, 0);

    auto arr = NdArray::createFrom(pixels);
    pixels =
        py.call("cv_input_test", arr).convert<NdArray>().convert<Pixels2D>();
    Image image2;
    image2.setFromPixes(pixels);
    gl::drawTexture(*image2.getTexture(), 0, 360);

#endif

#if 0
    Pixels2D pixels =
        py.call("test_cv2_video").convert<NdArray>().convert<Pixels2D>();
    tex->load(pixels.getData(0, 0), pixels.getWidth(), pixels.getHeight(), 0,
              0);
    gl::drawTexture(*tex, 0, 0);
#endif

#if 0
    float frame = math::lerp(getMouseX(), 0, getWidth(), 0, 1, true);
    Pixels2D pixels = py.call("test_cv2_video_frame", frame)
                          .convert<NdArray>()
                          .convert<Pixels2D>();
    tex->load(pixels.getData(0, 0), pixels.getWidth(), pixels.getHeight(), 0,
              0);
    gl::drawTexture(*tex, 0, 0);
#endif

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
