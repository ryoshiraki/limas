#pragma once
#include "gl/Tbo.h"
#include "gl/TextureBuffer.h"

namespace rs {
namespace gl {

template <typename T>
class IndirectionTexture {
 public:
  IndirectionTexture(size_t size) { resize(size); }

  void resize(size_t size) {
    size_ = size;
    tbo_data_.resize(size);
    tbo_ = gl::Tbo<T>::create();
    tbo_->setData(tbo_data_, GL_DYNAMIC_DRAW);
    tex_ = gl::TextureBuffer::create(*tbo_, GL_R32I);
  }

  void setIndirection(size_t location, int destination) {
    tbo_data_[location] = destination;
  }

  void update() { tbo_->update(tbo_data_); }

  gl::TextureBuffer::Ptr &getTexture() { return tex_; }
  const gl::TextureBuffer::Ptr &getTexture() const { return tex_; }

  size_t getSize() const { return size_; }

 private:
  size_t size_;
  typename gl::Tbo<T>::Ptr tbo_;
  std::vector<T> tbo_data_;
  gl::TextureBuffer::Ptr tex_;
};

}  // namespace gl
}  // namespace rs