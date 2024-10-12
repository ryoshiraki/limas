#pragma once
#include "gl/Shader.h"
#include "utils/FileObserver.h"

namespace limas {

class HotReloader : public FileObserver {
 public:
  HotReloader() {}

  void watchVertexShader(gl::Shader& shader, const std::string& filepath) {
    watch(filepath, [&](const std::string& e) {
      log::info("HotReloader")
          << "Vertex shader: " << e << " is reloaded." << log::end();
      shader.loadVertex(e);
      shader.link();
    });
  }

  void watchFragmentShader(gl::Shader& shader, const std::string& filepath) {
    watch(filepath, [&](const std::string& e) {
      log::info("HotReloader")
          << "Fragment shader: " << e << " is reloaded." << log::end();
      shader.loadFragment(e);
      shader.link();
    });
  }

  void watchGeometryShader(gl::Shader& shader, const std::string& filepath) {
    watch(filepath, [&](const std::string& e) {
      log::info("HotReloader")
          << "Geometry shader: " << e << " is reloaded." << log::end();
      shader.loadGeometry(e);
      shader.link();
    });
  }
};

}  // namespace limas
