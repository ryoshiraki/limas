#pragma once
#include "gl/TextureBase.h"
#include "graphics/Color.h"
#include "system/Logger.h"

#define POSITION_ATTRIBUTE 0
#define NORMAL_ATTRIBUTE 1
#define COLOR_ATTRIBUTE 2
#define TEXCOORD_ATTRIBUTE 3

#define TRANSLATION_ATTRIBUTE 4
#define ROTATION_ATTRIBUTE 5
#define SCALE_ATTRIBUTE 6
#define INSTANCE_COLOR_ATTRIBUTE 7

namespace limas {
namespace gl {

class ShaderBase {
 protected:
  struct ShaderProgram {
    GLuint id_;
    ShaderProgram() { id_ = glCreateProgram(); }
    ~ShaderProgram() { glDeleteProgram(id_); }
  };
  std::shared_ptr<ShaderProgram> program_;

  struct ShaderObject {
    GLuint id_;
    ShaderObject(GLenum type) { id_ = glCreateShader(type); }
    ~ShaderObject() { glDeleteShader(id_); }
  };
  std::map<GLenum, std::shared_ptr<ShaderObject>> shaders_;

 public:
  static inline ShaderBase* current_shader_ = nullptr;
  static ShaderBase* getCurrentShader() { return current_shader_; }

  virtual ~ShaderBase() {}

  bool load(const std::string& filepath, GLenum type) {
    std::string source = read(filepath);

    auto obj = std::make_shared<ShaderObject>(type);

    const char* c_source = source.c_str();
    glShaderSource(obj->id_, 1, &c_source, NULL);
    glCompileShader(obj->id_);

    GLint success;
    glGetShaderiv(obj->id_, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
      printCompileErrors(obj->id_);
      return false;
    }

    shaders_[type] = obj;

    return true;
  }

  bool link() {
    for (auto& shader : shaders_) {
      glAttachShader(getProgram(), shader.second->id_);
    }

    glLinkProgram(getProgram());

    for (auto& shader : shaders_) {
      glDetachShader(getProgram(), shader.second->id_);
    }

    if (isLinked()) return true;

    printLinkErrors();
    return false;
  }

  void bind() {
    glUseProgram(getProgram());
    current_shader_ = this;
  }

  void unbind() {
    glUseProgram(0);
    current_shader_ = nullptr;
  }

  void setTransformFeedbackVaryings(const std::vector<std::string>& varyings,
                                    GLenum mode = GL_INTERLEAVED_ATTRIBS) {
    std::vector<const GLchar*> char_ptrs;
    for (const auto& var : varyings) {
      char_ptrs.push_back(var.c_str());
    }

    glTransformFeedbackVaryings(getProgram(), char_ptrs.size(), &char_ptrs[0],
                                mode);
  }

  GLint getUniformLocation(const std::string& name) const {
    return glGetUniformLocation(getProgram(), name.c_str());
  }

  GLint getAttributeLocation(const std::string& name) const {
    return glGetAttribLocation(getProgram(), name.c_str());
  }

  GLint getUniformBlockIndex(const std::string& name) const {
    return glGetUniformBlockIndex(getProgram(), name.c_str());
  }

  void setUniform1f(const std::string& name, GLfloat data) {
    glUniform1f(getUniformLocation(name), data);
  }

  void setUniform1fv(const std::string& name, GLfloat* data, GLsizei count) {
    glUniform1fv(getUniformLocation(name), count, data);
  }

  void setUniform2f(const std::string& name, const glm::vec2& data) {
    glUniform2fv(getUniformLocation(name), 1, &data[0]);
  }

  void setUniform2fv(const std::string& name, glm::vec2* data, GLsizei count) {
    glUniform2fv(getUniformLocation(name), count, glm::value_ptr(data[0]));
  }

  void setUniform3f(const std::string& name, const glm::vec3& data) {
    glUniform3fv(getUniformLocation(name), 1, &data[0]);
  }

  void setUniform3fv(const std::string& name, glm::vec3* data, GLsizei count) {
    glUniform3fv(getUniformLocation(name), count, glm::value_ptr(data[0]));
  }

  void setUniform4f(const std::string& name, const glm::vec4& data) {
    glUniform4fv(getUniformLocation(name), 1, &data[0]);
  }

  void setUniform4fv(const std::string& name, glm::vec4* data, GLsizei count) {
    glUniform4fv(getUniformLocation(name), count, glm::value_ptr(data[0]));
  }

  void setUniform4f(const std::string& name, const Color& data) {
    glUniform4fv(getUniformLocation(name), 1, &data[0]);
  }

  void setUniform1i(const std::string& name, GLint data) {
    glUniform1i(getUniformLocation(name), data);
  }

  void setUniform1iv(const std::string& name, GLint* data, GLsizei count) {
    glUniform1iv(getUniformLocation(name), count, data);
  }

  void setUniform2i(const std::string& name, const glm::ivec2& data) {
    glUniform2iv(getUniformLocation(name), 1, &data[0]);
  }

  void setUniform2iv(const std::string& name, glm::ivec2* data, GLsizei count) {
    glUniform2iv(getUniformLocation(name), count, glm::value_ptr(data[0]));
  }

  void setUniform3i(const std::string& name, const glm::ivec3& data) {
    glUniform3iv(getUniformLocation(name), 1, &data[0]);
  }

  void setUniform3iv(const std::string& name, glm::ivec3* data, GLsizei count) {
    glUniform3iv(getUniformLocation(name), count, glm::value_ptr(data[0]));
  }

  void setUniform4i(const std::string& name, const glm::ivec4& data) {
    glUniform4iv(getUniformLocation(name), 1, &data[0]);
  }

  void setUniform4iv(const std::string& name, glm::ivec4* data, GLsizei count) {
    glUniform4iv(getUniformLocation(name), count, glm::value_ptr(data[0]));
  }

  void setUniform1ui(const std::string& name, GLuint data) {
    glUniform1ui(getUniformLocation(name), data);
  }

  void setUniform1uiv(const std::string& name, GLuint* data, GLsizei count) {
    glUniform1uiv(getUniformLocation(name), count, data);
  }

  void setUniform2ui(const std::string& name, const glm::uvec2& data) {
    glUniform2uiv(getUniformLocation(name), 1, &data[0]);
  }

  void setUniform2uiv(const std::string& name, glm::uvec2* data,
                      GLsizei count) {
    glUniform2uiv(getUniformLocation(name), count, glm::value_ptr(data[0]));
  }

  void setUniform3ui(const std::string& name, const glm::uvec3& data) {
    glUniform3uiv(getUniformLocation(name), 1, &data[0]);
  }

  void setUniform3uiv(const std::string& name, glm::uvec3* data,
                      GLsizei count) {
    glUniform3uiv(getUniformLocation(name), count, glm::value_ptr(data[0]));
  }

  void setUniform4ui(const std::string& name, const glm::uvec4& data) {
    glUniform4uiv(getUniformLocation(name), 1, &data[0]);
  }

  void setUniform4uiv(const std::string& name, glm::uvec4* data,
                      GLsizei count) {
    glUniform4uiv(getUniformLocation(name), count, glm::value_ptr(data[0]));
  }

  void setUniformMatrix2f(const std::string& name, const glm::mat2& data) {
    glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &data[0][0]);
  }

  void setUniformMatrix3f(const std::string& name, const glm::mat3& data) {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &data[0][0]);
  }

  void setUniformMatrix4f(const std::string& name, const glm::mat4& data) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &data[0][0]);
  }

  void setUniformTexture(const std::string& name, GLenum target, GLuint id,
                         int index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(target, id);
    setUniform1i(name, index);
  }

  void setUniformTexture(const std::string& name, const TextureBase& t,
                         int index) {
    setUniformTexture(name, t.getTarget(), t.getID(), index);
  }

  void bindUniformBlock(const std::string& name, const GLuint index) const {
    const auto block_index = getUniformBlockIndex(name);
    if (block_index != GL_INVALID_INDEX) {
      glUniformBlockBinding(getProgram(), block_index, index);
    } else {
      log::warn("Shader") << "bindUniformBlock: GL_INVALID_INDEX" << log::end();
    }
  }

  GLuint getProgram() const { return program_->id_; }

  bool isLinked() const {
    GLint b_linked = 0;
    glGetProgramiv(getProgram(), GL_LINK_STATUS, &b_linked);
    return b_linked == GL_TRUE;
  }

 protected:
  ShaderBase() : program_(std::make_shared<ShaderProgram>()) {}

 private:
  std::string read(const std::string& filepath) const {
    std::ifstream ifs(filepath);
    if (!ifs)
      log::error("Shader") << "cannot read shader file from " << filepath
                           << log::end();

    std::ostringstream source;

    std::string line;
    while (std::getline(ifs, line)) {
      if (line.substr(0, 9) == "#resource") {
        std::string included_file = line.substr(10);  // "filename"
        included_file =
            included_file.substr(1, included_file.size() - 2);  // filename
        source << read(fs::getCommonResourcesPath() + "shaders/" +
                       included_file)
               << "\n";
      } else if (line.substr(0, 8) == "#include") {
        std::string included_file = line.substr(9);  // "filename"
        included_file =
            included_file.substr(1, included_file.size() - 2);  // filename

        std::string parent = fs::getParent(filepath);
        source << read(parent + "/" + included_file) << "\n";
      } else {
        source << line << "\n";
      }
    }

    ifs.close();
    return source.str();
  }

  void printCompileErrors(GLuint shader) const {
    GLint size;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);
    std::vector<GLchar> error_log(size);
    glGetShaderInfoLog(shader, size, &size, &error_log[0]);
    std::string error_log_str(error_log.begin(), error_log.end());
    log::error("Shader") << "ERROR::SHADER_COMPILATION_ERROR\n"
                         << error_log_str << log::end();
  }

  void printLinkErrors() const {
    GLint size;
    glGetProgramiv(getProgram(), GL_INFO_LOG_LENGTH, &size);
    std::vector<GLchar> error_log(size);
    glGetProgramInfoLog(getProgram(), size, &size, &error_log[0]);
    std::string error_log_str(error_log.begin(), error_log.end());
    log::error("Shader") << "ERROR::PROGRAM_LINKING_ERROR\n"
                         << error_log_str << log::end();
  }
};

}  // namespace gl
}  // namespace limas
