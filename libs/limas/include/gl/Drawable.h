
#pragma once
#include "geom/Mesh.h"
#include "gl/Ibo.h"
#include "gl/Shader.h"
#include "gl/Vao.h"

namespace limas {
namespace gl {

class AttributeBase {
 public:
  using Ptr = shared_ptr<AttributeBase>;

  AttributeBase(GLuint location, GLuint dim, GLenum type)
      : location_(location), dim_(dim), type_(type) {}
  virtual ~AttributeBase() {}

  GLuint getLocation() const { return location_; }
  GLuint getDim() const { return dim_; }
  GLenum getType() const { return type_; }

  virtual void allocate(const void *data, size_t size) = 0;
  virtual void update(const void *data, size_t size) = 0;
  virtual void bind() const = 0;
  virtual void unbind() const = 0;
  virtual GLuint getID() const = 0;
  virtual GLsizei getSize() const = 0;
  virtual GLsizei getStride() const = 0;

 protected:
  GLuint location_;
  GLuint dim_;
  GLenum type_;
};

template <typename T>
class Attribute : public AttributeBase {
 public:
  Attribute(GLuint index, const vector<T> &data, GLuint size, GLenum type)
      : AttributeBase(index, size, type) {
    allocate(data);
  }

  void allocate(const std::vector<T> &data) {
    allocate(data.data(), data.size());
  }

  void allocate(const void *data, size_t size) override {
    vbo_.allocate(data, size);
  }

  void update(const std::vector<T> &data) { update(data.data(), data.size()); }
  void update(const void *data, size_t size) override {
    vbo_.update(data, size);
  }

  void bind() const override { vbo_.bind(); }
  void unbind() const override { vbo_.unbind(); }
  GLsizei getSize() const override { return vbo_.getSize(); }
  GLuint getID() const override { return vbo_.getID(); }
  GLsizei getStride() const { return vbo_.getStride(); }

 protected:
  gl::Vbo<T> vbo_;
};

class Drawable {
 protected:
 public:
  Drawable() {}

  template <typename T>
  void addAttribute(GLuint location, const std::vector<T> &data, GLuint dim,
                    GLenum type) {
    std::shared_ptr<Attribute<T>> attr =
        std::make_shared<Attribute<T>>(location, data, dim, type);
    attributes_[location] = attr;

    vao_.bindVbo(attr->getID(), location, dim, attr->getType(), GL_FALSE,
                 attr->getStride());
  }

  void removeAttribute(GLuint location) {
    auto it = attributes_.find(location);
    if (it != attributes_.end()) {
      vao_.unbindVbo(location);
      attributes_.erase(it);
    } else {
      logger::warn("removeAttribute()")
          << "attribute " << location << " not found" << logger::end();
    }
  }

  template <typename T>
  void updateAttribute(GLuint location, const std::vector<T> &data) {
    auto it = attributes_.find(location);
    if (it != attributes_.end()) {
      auto &attr = it->second;
      attr->update(data.data(), data.size());
    } else {
      logger::warn("updateAttribute()")
          << "attribute " << location << " not found" << logger::end();
    }
  }

  void updateIndices(const std::vector<GLuint> &data) {
    if (ibo_.getSize() != data.size()) {
      ibo_.allocate(data);
      vao_.bindIbo(ibo_);
    } else {
      ibo_.update(data);
    }
  }

  void enableAttribute(GLuint location) {
    auto it = attributes_.find(location);
    if (it != attributes_.end()) {
      auto &attr = it->second;
      vao_.bindVbo(attr->getID(), location, attr->getDim(), attr->getType(),
                   GL_FALSE, attr->getStride());
    } else {
      logger::warn("enableAttribute()")
          << "attribute " << location << " not found" << logger::end();
    }
  }

  void disableAttribute(GLuint location) {
    if (attributes_.find(location) != attributes_.end()) {
      vao_.unbindVbo(location);
    } else {
      logger::warn("disableAttribute()")
          << "attribute " << location << " not found" << logger::end();
    }
  }

  void draw(GLenum mode, GLsizei count, GLsizei start = 0) const {
    if (attributes_.size() == 0) return;

    if (ibo_.getSize()) {
      vao_.drawElements(mode, count);
    } else {
      vao_.drawArrays(mode, start, count);
    }
  }

  void drawInstanced(GLenum mode, GLsizei instance_count, GLsizei count,
                     GLsizei start = 0) const {
    if (attributes_.size() == 0) return;

    if (ibo_.getSize()) {
      vao_.drawElementsInstanced(mode, count, instance_count);
    } else {
      vao_.drawArraysInstanced(mode, start, count, instance_count);
    }
  }

  Vao &getVao() { return vao_; };
  const Vao &getVao() const { return vao_; };

  bool hasAttribute(GLuint index) const {
    return attributes_.find(index) != attributes_.end();
  }

  bool isAttributeEnabled(GLuint index) const {
    return vao_.isAttributeEnabled(index);
  }

 protected:
  gl::Vao vao_;
  std::map<GLuint, AttributeBase::Ptr> attributes_;
  gl::Ibo<GLuint> ibo_;
};

}  // namespace gl
}  // namespace limas