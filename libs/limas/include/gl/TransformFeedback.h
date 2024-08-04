#pragma once

namespace limas {
namespace gl {

class TransformFeedback {
 private:
  struct BufferData {
    GLuint id_;
    BufferData() { glGenTransformFeedbacks(1, &id_); }
    ~BufferData() { glDeleteTransformFeedbacks(1, &id_); }
  };
  std::shared_ptr<BufferData> data_;

 public:
  TransformFeedback() : data_(std::make_shared<BufferData>()) {}
  virtual ~TransformFeedback() {}

  void bind() const { glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, getID()); }
  void unbind() const { glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0); }

  void bindBuffer(GLuint index, GLuint id) {
    bind();
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, id);
    unbind();
  }

  void begin(GLenum mode) const {
    glEnable(GL_RASTERIZER_DISCARD);
    bind();
    glBeginTransformFeedback(mode);
  }

  void end() const {
    glEndTransformFeedback();
    unbind();
    glDisable(GL_RASTERIZER_DISCARD);
  }

  void draw(GLenum mode) { glDrawTransformFeedback(mode, getID()); }

  GLuint getID() const { return data_->id_; }
};

}  // namespace gl
}  // namespace limas
