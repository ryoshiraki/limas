#pragma once
#include "gl/Shader.h"
#include "utils/FileObserver.h"

namespace limas {

class HotReloader : public FileObserver {
 public:
  HotReloader() {}

  void watchVertexShader(gl::Shader& shader, const std::string& filepath) {
    watch(filepath, [&](const std::string& e) {
      shader.loadVertex(e);
      shader.link();
    });
  }

  void watchFragmentShader(gl::Shader& shader, const std::string& filepath) {
    watch(filepath, [&](const std::string& e) {
      shader.loadFragment(e);
      shader.link();
    });
  }
};

}  // namespace limas
