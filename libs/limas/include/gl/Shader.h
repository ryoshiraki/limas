#pragma once
#include "gl/ShaderBase.h"

namespace limas {
namespace gl {

class Shader : public ShaderBase {
 public:
  Shader() {}

  bool load(const std::string& vertex, const std::string& fragment) {
    if (loadVertex(vertex) && loadFragment(fragment)) {
      return link();
    }
    return false;
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
