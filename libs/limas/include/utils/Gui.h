#pragma once
#include "app/Window.h"
#include "graphics/Color.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "system/Noncopyable.h"
#include "system/Observable.h"
#include "type/ParameterGroup.h"

namespace rs {
namespace ui = ImGui;
}

namespace ImGui {

static rs::CoreObservable observables_;

inline void MouseButtonCallback(GLFWwindow *window, int button, int action,
                                int mods) {
  ImGuiIO &io = GetIO();
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
  if (io.WantCaptureMouse == true) {
    rs::MouseEventArgs e{io.MousePos.x, io.MousePos.y, button};
    if (action == GLFW_PRESS)
      observables_.mouse_pressed.notify(e);
    else if (action == GLFW_RELEASE)
      observables_.mouse_released.notify(e);
  }
}

inline void initialize(rs::Window::Ptr &window) {
  IMGUI_CHECKVERSION();
  CreateContext();
  ImGuiIO &io = GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
  StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window->getHandle(), true);
  const char *glsl_version = "#version 410";
  ImGui_ImplOpenGL3_Init(glsl_version);

  glfwSetMouseButtonCallback(window->getHandle(), MouseButtonCallback);

  window->setOnLoopBegin([&window](const rs::EventArgs &) {
    ImGuiIO &io = GetIO();
    io.MousePos = ImVec2(window->getMouseX(), window->getMouseY());
    io.MouseDown[0] = window->getMouseButton() == rs::MOUSE_BUTTON_LEFT;
    io.MouseDown[1] = window->getMouseButton() == rs::MOUSE_BUTTON_RIGHT;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();
  });

  window->setOnLoopEnd([&window](const rs::EventArgs &) { EndFrame(); });
}

inline void draw() {
  Render();
  ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

template <typename Callback>
inline rs::Connection setOnMousePressed(Callback callback) {
  std::function<void(const rs::MouseEventArgs &)> func = callback;
  return observables_.mouse_pressed.addObserver(callback);
}

template <typename Callback>
inline rs::Connection setOnMouseReleased(Callback callback) {
  std::function<void(const rs::MouseEventArgs &)> func = callback;
  return observables_.mouse_released.addObserver(callback);
}

#pragma mark INPUT HELPER

inline bool InputTextString(const std::string &name, std::string *v,
                            ImGuiInputTextFlags flags = 0) {
  char buffer[256];
  strncpy(buffer, v->c_str(), sizeof(buffer));
  buffer[sizeof(buffer) - 1] = 0;  // 終端文字を確実に設定

  if (ImGui::InputText(name.c_str(), buffer, sizeof(buffer), flags)) {
    *v = buffer;
    return true;
  }
  return false;
}

template <typename T, int N>
inline bool SliderVec(const std::string &label, glm::vec<N, T> *v,
                      const glm::vec<N, T> &min, const glm::vec<N, T> &max) {
  ImGuiDataType type;
  if constexpr (std::is_same_v<T, float>) {
    type = ImGuiDataType_Float;
  } else if constexpr (std::is_same_v<T, int>) {
    type = ImGuiDataType_S32;
  }

  bool b_changed = false;
  for (int i = 0; i < N; ++i) {
    if (ImGui::SliderScalar((label + "[" + std::to_string(i) + "]").c_str(),
                            type, &(*v)[i], &min[i], &max[i]))
      b_changed = true;
  }
  return b_changed;
}

template <typename T>
inline bool SliderColor(const std::string &label, rs::BaseColor<T> *v,
                        const rs::BaseColor<T> &min,
                        const rs::BaseColor<T> &max) {
  ImGuiDataType type;
  if constexpr (std::is_same_v<T, unsigned char>) {
    type = ImGuiDataType_U8;
  } else if constexpr (std::is_same_v<T, float>) {
    type = ImGuiDataType_Float;
  } else if constexpr (std::is_same_v<T, int>) {
    type = ImGuiDataType_S32;
  }

  bool b_changed = false;
  for (int i = 0; i < 4; ++i) {
    if (ImGui::SliderScalar((label + "[" + std::to_string(i) + "]").c_str(),
                            type, &(*v)[i], &min[i], &max[i]))
      b_changed = true;
  }
  return b_changed;
}

#pragma mark PARAMETER

template <typename T>
inline bool addParam(rs::Parameter<T> &param, T min, T max) {
  ImGuiDataType type;
  if constexpr (std::is_same_v<T, char>) {
    type = std::is_signed_v<T> ? ImGuiDataType_S8 : ImGuiDataType_U8;
  } else if constexpr (std::is_same_v<T, short>) {
    type = std::is_signed_v<T> ? ImGuiDataType_S16 : ImGuiDataType_U16;
  } else if constexpr (std::is_same_v<T, int>) {
    type = std::is_signed_v<T> ? ImGuiDataType_S32 : ImGuiDataType_U32;
  } else if constexpr (std::is_same_v<T, long long>) {
    type = std::is_signed_v<T> ? ImGuiDataType_S64 : ImGuiDataType_U64;
  } else if constexpr (std::is_same_v<T, float>) {
    type = ImGuiDataType_Float;
  } else if constexpr (std::is_same_v<T, double>) {
    type = ImGuiDataType_Double;
  }

  T tmp = param.getValue();
  if (ImGui::SliderScalar(param.getName().c_str(), type, &tmp, &min, &max, NULL,
                          0)) {
    param.setValue(tmp);
    return true;
  }
  return false;
}

template <typename T>
inline bool addParam(rs::Parameter<T> &param) {
  return addParam<T>(param, param.getMin(), param.getMax());
}

template <>
inline bool addParam<bool>(rs::Parameter<bool> &param) {
  bool tmp = param.getValue();
  if (Checkbox(param.getName().c_str(), &tmp)) {
    param.setValue(tmp);
    return true;
  }
  return false;
}

template <>
inline bool addParam<std::string>(rs::Parameter<std::string> &param) {
  std::string tmp = param.getValue();
  if (InputTextString(param.getName().c_str(), &tmp)) {
    param.setValue(tmp);
    return true;
  }
  return false;
}

template <typename T, int N>
inline bool addParam(rs::Parameter<glm::vec<N, T>> &param,
                     const glm::vec<N, T> &min, const glm::vec<N, T> &max) {
  ImGuiDataType type;
  if constexpr (std::is_same_v<T, float>) {
    type = ImGuiDataType_Float;
  } else if constexpr (std::is_same_v<T, int>) {
    type = ImGuiDataType_S32;
  }

  glm::vec<N, T> tmp = param.getValue();
  if (SliderVec(param.getName(), &tmp, min, max)) {
    param.setValue(tmp);
    return true;
  }

  return false;
}

template <typename T, int N>
inline bool addParam(rs::Parameter<glm::vec<N, T>> &param) {
  return addParam<T, N>(param, param.getMin(), param.getMax());
}

inline void addParams(rs::ParameterGroup &params) {
  if (CollapsingHeader(params.getName().c_str(),
                       ImGuiTreeNodeFlags_DefaultOpen)) {
    for (auto &param : params.getParams()) {
      auto type = param->getType();
      if (type == typeid(bool)) {
        addParam(param->as<bool>());
      } else if (type == typeid(char)) {
        addParam(param->as<char>());
      } else if (type == typeid(int)) {
        addParam(param->as<int>());
      } else if (type == typeid(float)) {
        addParam(param->as<float>());
      } else if (type == typeid(double)) {
        addParam(param->as<double>());
      } else if (type == typeid(glm::vec2)) {
        addParam(param->as<glm::vec2>());
      } else if (type == typeid(glm::vec3)) {
        addParam(param->as<glm::vec3>());
      } else if (type == typeid(glm::vec4)) {
        addParam(param->as<glm::vec4>());
      } else if (type == typeid(std::string)) {
        addParam(param->as<std::string>());
      } else {
        rs::log::error("GUI") << type.name() << " of " << param->getName()
                              << " is not supported now" << rs::log::end();
      }
    }
  }
}

#pragma mark SCOPED GUI

struct ScopedWindow : public rs::Noncopyable {
  ScopedWindow(const std::string &label, int x, int y,
               ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize) {
    SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
    Begin(label.c_str(), nullptr, flags);
  }
  ~ScopedWindow() { End(); }
};

struct ScopedGroup : public rs::Noncopyable {
  ScopedGroup() { BeginGroup(); }
  ~ScopedGroup() { EndGroup(); }
};

struct ScopedTreeNode : public rs::Noncopyable {
  ScopedTreeNode(const std::string &label) { TreeNode(label.c_str()); }
  ~ScopedTreeNode() { TreePop(); }

 protected:
  bool b_opened_;
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
