#pragma once
#include "gl/ShaderBase.h"
#include "system/Logger.h"

namespace rs {
namespace gl {

class ComputeShader : public ShaderBase {
 public:
  ComputeShader() {}
  virtual ~ComputeShader();

  bool load(const std::string& filepath) {
    return load(filepath, GL_COMPUTE_SHADER) && link();
  }

  void dispatch(int x = 1, int y = 1, int z = 1, bool block = true) {
    glDispatchCompute(x, y, z);
    if (block) glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }

  static void printLimits() {
    const std::function<int(GLenum)> getInt = [](GLenum E) {
      int i;
      glGetIntegerv(E, &i);
      return i;
    };

    int max_work_groups;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_work_groups);
    int max_work_size;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &max_work_size);

    log::info("ComputeShader")
        << "Max SSBO: " << getInt(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS) << "\n"
        << "Max SSBO Block-Size: " << getInt(GL_MAX_SHADER_STORAGE_BLOCK_SIZE)
        << "\n"
        << "Max Compute Shader Storage Blocks: "
        << getInt(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS) << "\n"
        << "Max Shared Storage Size: "
        << getInt(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE) << "\n"
        << "Max Work Groups: " << m << "\n"
        << "Max Local Size: " << m << log::end();
  }
};

}  // namespace gl
}  // namespace rs
