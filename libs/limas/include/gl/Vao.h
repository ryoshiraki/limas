#pragma once
#include "Ibo.h"
#include "Vbo.h"

namespace limas {
namespace gl {

class Vao {
 private:
  struct VaoData {
    GLuint id_;
    VaoData() { glGenVertexArrays(1, &id_); }
    ~VaoData() { glDeleteVertexArrays(1, &id_); }
  };
  std::shared_ptr<VaoData> data_;

 public:
  Vao() : data_(std::make_shared<VaoData>()), b_ibo_enabled_(false) {}
  virtual ~Vao() {}

  void bind() const { glBindVertexArray(data_->id_); }
  void unbind() const { glBindVertexArray(0); }

  void bindVbo(GLuint id, GLuint location, GLint dim, GLenum type,
               GLboolean normalized, GLsizei stride, const void* offset = 0) {
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, dim, type, GL_FALSE, stride, offset);
    unbind();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    enabled_attributes_.insert(location);
  }

  template <typename T>
  void bindVbo(Vbo<T>& buf, GLuint location, GLint dim, GLenum type,
               const void* offset = 0) {
    bindVbo(buf.getID(), location, dim, type, GL_FALSE, buf.getStride(),
            offset);
  }

  void unbindVbo(GLuint location) {
    bind();
    glDisableVertexAttribArray(location);
    unbind();
    enabled_attributes_.erase(location);
  }

  void bindIbo(GLuint id) {
    bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    unbind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    b_ibo_enabled_ = true;
  }

  template <typename T>
  void bindIbo(Ibo<T>& buf) {
    bindIbo(buf.getID());
  }

  void unbindIbo() {
    bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    unbind();
    b_ibo_enabled_ = false;
  }

  void setVertexAttribDivisor(GLuint location, GLuint divisor) {
    bind();
    glVertexAttribDivisor(location, divisor);
    unbind();
  }

  void drawArrays(GLenum mode, GLsizei first, GLsizei count) const {
    bind();
    glDrawArrays(mode, first, count);
    unbind();
  }

  void drawElements(GLenum mode, GLsizei count, GLenum type = GL_UNSIGNED_INT,
                    const GLvoid* indices = 0) const {
    bind();
    glDrawElements(mode, count, type, indices);
    unbind();
  }

  void drawArraysInstanced(GLenum mode, GLsizei first, GLsizei count,
                           GLsizei instancecount) const {
    bind();
    glDrawArraysInstanced(mode, first, count, instancecount);
    unbind();
  }

  void drawElementsInstanced(GLenum mode, GLsizei count,
                             GLsizei instancecount) const {
    bind();
    glDrawElementsInstanced(mode, count, GL_UNSIGNED_INT, nullptr,
                            instancecount);
    unbind();
  }

  bool isAttributeEnabled(GLuint index) const {
    return enabled_attributes_.find(index) != enabled_attributes_.end();
  }

  bool isIboEnabled() const { return b_ibo_enabled_; }

 private:
  bool b_ibo_enabled_;
  std::set<GLuint> enabled_attributes_;
};

}  // namespace gl
}  // namespace limas
