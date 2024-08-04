#pragma once
#include "app/Window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "system/Noncopyable.h"
namespace rs {
namespace ui = ImGui;
}

namespace ImGui {

static void initialize(rs::Window::Ptr &window) {
  IMGUI_CHECKVERSION();
  CreateContext();
  ImGuiIO &io = GetIO();
  (void)io;
  io.MouseDrawCursor = false;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
  StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window->getHandle(), true);
  const char *glsl_version = "#version 410";
  ImGui_ImplOpenGL3_Init(glsl_version);

  window->setOnLoopBegin([](const rs::EventArgs &) {
    ImGuiIO &io = GetIO();
    // io.DeltaTime = 0;
    // io.MousePos = ImVec2((float)0, (float)0);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();
  });
}

static void draw() {
  Render();
  ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

struct ScopedWindow : public rs::Noncopyable {
  ScopedWindow(const std::string &name, ImGuiWindowFlags flags = 0) {
    Begin(name.c_str(), nullptr, flags);
  }
  ~ScopedWindow() { End(); }
};

struct ScopedMenuBar : public rs::Noncopyable {
  ScopedMenuBar() { b_opened_ = BeginMenuBar(); }
  ~ScopedMenuBar() {
    if (b_opened_) EndMenuBar();
  }

 protected:
  bool b_opened_;
};

struct ScopedMainMenuBar : public rs::Noncopyable {
  ScopedMainMenuBar() { b_opened_ = BeginMainMenuBar(); }
  ~ScopedMainMenuBar() {
    if (b_opened_) EndMainMenuBar();
  }

 protected:
  bool b_opened_;
};

}  // namespace ImGui
