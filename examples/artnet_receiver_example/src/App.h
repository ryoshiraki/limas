#pragma once
#include "app/BaseApp.h"
#include "gl/InstancedVboMesh.h"
#include "net/ArtnetReceiver.h"
#include "net/OscSender.h"
#include "primitives/Grid.h"
#include "primitives/Rectangle.h"

namespace rs {

using namespace std;

class App : public BaseApp {
  static const int NUM_DMX_DATA = 512;
  static const int NUM_COLS = 32;
  static const int NUM_ROWS = 16;
  int grid_size = 30;

 public:
  using BaseApp::BaseApp;

  boost::asio::io_service io_service;
  net::ArtnetReceiver receiver;
  net::OscSender sender;

  gl::InstancedVboMesh mesh;
  gl::VboMesh grid;

  void setup() {
    setFPS(30.0f);
    setVerticalSync(true);

    setWindowSize(grid_size * NUM_COLS, grid_size * NUM_ROWS);

    mesh = prim::Rectangle(0, 0, grid_size, grid_size);
    mesh.allocateInstances(NUM_DMX_DATA);
    auto &translations = mesh.getTranslations();
    for (int iy = 0; iy < NUM_ROWS; iy++) {
      for (int ix = 0; ix < NUM_COLS; ix++) {
        int i = iy * NUM_COLS + ix;
        translations[i] = glm::vec3(grid_size + ix * grid_size,
                                    grid_size + iy * grid_size, 0);
      }
    }
    mesh.updateTranslations();

    grid = prim::Grid(getWidth(), getHeight(), grid_size, grid_size);

    receiver.start(io_service);
    sender.setup("localhost", 12345);
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    auto data = receiver.getValues(1, 512);
    sender.send("/dmx", data);
    
    auto &colors = mesh.getInstanceColors();
    for (int i = 0; i < NUM_DMX_DATA; i++) {
      colors[i] = glm::vec4(glm::vec3(data[i] / 255.0), 1);
    }
    mesh.updateInstanceColors();

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    
    gl::setColor(1, 1, 1, 1);
    gl::drawMeshInstanced(mesh, GL_TRIANGLE_FAN, mesh.getNumInstances());
    gl::drawMesh(grid, GL_LINES);
    for (int iy = 0; iy < NUM_ROWS; iy++) {
      for (int ix = 0; ix < NUM_COLS; ix++) {
        int i = iy * NUM_COLS + ix;
        gl::drawBitmapString(std::to_string(i), ix * grid_size + 1,
                             iy * grid_size);
        gl::drawBitmapString(std::to_string(data[i]), ix * grid_size + 1,
                             iy * grid_size + 6);
      }
    }

    gl::drawBitmapString("FPS:" + util::toString(getFPS(), 2, 6, '0'), 5, 5);
    gl::drawBitmapString(
        "MEM:" + util::toString(getMemoryUsage(), 2, 6, '0') + " MB", 5, 15);
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
