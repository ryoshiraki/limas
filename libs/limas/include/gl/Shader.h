#pragma once
#include "gl/ShaderBase.h"

namespace limas {
namespace gl {

class Shader : public ShaderBase {
 public:
  Shader() {}

  bool load(const std::string& vertex, const std::string& fragment,
            const std::string& geometry = "",
            const std::string& tess_control = "",
            const std::string& tess_evaluation = "") {
    bool success = true;
    success &= loadVertex(vertex);
    success &= loadFragment(fragment);
    if (!geometry.empty()) success &= loadGeometry(geometry);
    if (!tess_control.empty()) success &= loadTessControl(tess_control);
    if (!tess_evaluation.empty())
      success &= loadTessEvaluation(tess_evaluation);
    return success && link();
  }

  bool loadVertex(const std::string& filepath) {
    return ShaderBase::load(filepath, GL_VERTEX_SHADER);
  }

  bool loadFragment(const std::string& filepath) {
    return ShaderBase::load(filepath, GL_FRAGMENT_SHADER);
  }

  bool loadGeometry(const std::string& filepath) {
    return ShaderBase::load(filepath, GL_GEOMETRY_SHADER);
  }

  bool loadTessControl(const std::string& filepath) {
    return ShaderBase::load(filepath, GL_TESS_CONTROL_SHADER);
  }

  bool loadTessEvaluation(const std::string& filepath) {
    return ShaderBase::load(filepath, GL_TESS_EVALUATION_SHADER);
  }
};

}  // namespace gl
}  // namespace limas
