#pragma once
#include "gl/Shader.h"
#include "utils/FileObserver.h"

namespace limas {

class HotReloader : public FileObserver {
 public:
  HotReloader() {}

  bool loadAndWatchShader(gl::Shader& shader, const std::string& vertex_path,
                          const std::string& fragment_path,
                          const std::string& geometry_path = "") {
    shader.loadVertex(vertex_path);
    shader.loadFragment(fragment_path);
    if (!geometry_path.empty()) shader.loadGeometry(geometry_path);
    if (!shader.link()) return false;
    watchShader(shader, vertex_path, fragment_path, geometry_path);
    return true;
  }

  void watchShader(gl::Shader& shader, const std::string& vertex_path,
                   const std::string& fragment_path,
                   const std::string& geometry_path = "") {
    watchVertexShader(shader, vertex_path);
    watchFragmentShader(shader, fragment_path);
    if (!geometry_path.empty()) watchGeometryShader(shader, geometry_path);
  }

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
