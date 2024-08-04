#pragma once
#include "app/BaseApp.h"
#include "net/UdpClient.h"
#include "net/UdpServer.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  boost::asio::io_service io_service;
  net::UdpServer::Ptr server;
  net::UdpClient::Ptr client;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    // server = net::UdpServer::create(io_service, 12345);
    client = net::UdpClient::create(io_service, "192.168.1.188", 12345);
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    // client->send("test");

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    gl::popMatrix();
  }

  void keyPressed(const rs::KeyEventArgs &e) {
    if (e.key == ' ') client->send("test");
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
