#pragma once
#include <boost/thread.hpp>

#include "app/BaseApp.h"
#include "net/UdpClient.h"
#include "net/UdpServer.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  std::thread thread;
  boost::asio::io_service io_service;
  net::UdpServer::Ptr server;
  string message;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    server = net::UdpServer::create(io_service, 12345);
    server->setCallback([&](const string &m) {
      cout << m << endl;
      message = m;
    });
    thread = std::thread([&] { io_service.run(); });
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    gl::setColor(1, 1, 1, 1);
    gl::drawBitmapString(message, 20, 20);
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
