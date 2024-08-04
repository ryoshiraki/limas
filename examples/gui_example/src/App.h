#pragma once
#include "Gui.h"
#include "app/BaseApp.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace rs {

using namespace std;

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    ui::initialize(getMainWindow());
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());

    if (ui::Begin("tesg")) {
      if (ui::Button("Add")) {
        log::info() << "add" << log::end();
      }
      ui::End();
    }

    {
      ui::ScopedWindow wind("test", ImGuiWindowFlags_MenuBar);
      {
        ui::ScopedMenuBar menuBar;

        if (ui::BeginMenu("Edit")) {
          ui::MenuItem("Copy");
          ui::MenuItem("Cut");
          ui::MenuItem("Paste");
          ui::EndMenu();
        }
      }
      ui::Text("Window Content");
    }

    {
      ui::ScopedMainMenuBar menuBar;

      // add a file menu
      if (ui::BeginMenu("File")) {
        ui::MenuItem("Open");
        ui::MenuItem("Save");
        ui::MenuItem("Save As");
        ui::EndMenu();
      }
    }

    ui::ShowDemoWindow();

    ui::draw();
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
