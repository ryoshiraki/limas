#pragma once
#include "app/BaseApp.h"
#include "net/HttpClient.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  std::thread thread;
  boost::asio::io_service io_service;
  net::HttpClient::Ptr http_client;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    http_client = net::HttpClient::create(io_service, "httpbin.org");
    // thread = std::thread([&] { io_service.run(); });
    io_service.run();
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    gl::popMatrix();
  }

  void keyPressed(const rs::KeyEventArgs &e) {
    http_client->request("GET", "/get", "");
    io_service.run();
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
