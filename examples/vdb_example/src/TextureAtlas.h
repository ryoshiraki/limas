#pragma once
#include "SparseVolume/Leaf.h"
#include "gl/Texture3D.h"
#include "graphics/Pixels.h"

namespace rs {

//! Compute the next highest power of 2 of 32-bit unsigned int
static unsigned int roundUpToNearestPowerOfTwo(unsigned int v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

class TextureAtlas {
 public:
  using Ptr = std::shared_ptr<TextureAtlas>;

  TextureAtlas(int max_item_count, int item_width, int channel_count)
      : item_count_(0), item_width_(item_width) {
    item_count_per_dim_.x = ceil(pow((double)max_item_count, 1.0 / 3.0));
    item_count_per_dim_.x = roundUpToNearestPowerOfTwo(item_count_per_dim_.x);

    item_count_per_dim_.y =
        ceil(sqrt((double)max_item_count / (double)item_count_per_dim_.x));
    item_count_per_dim_.y = roundUpToNearestPowerOfTwo(item_count_per_dim_.y);

    item_count_per_dim_.z =
        ceil((double)max_item_count /
             (double)(item_count_per_dim_.x * item_count_per_dim_.y));
    item_count_per_dim_.z = roundUpToNearestPowerOfTwo(item_count_per_dim_.z);

    pixels_.allocate(item_count_per_dim_.x * item_width_,
                     item_count_per_dim_.y * item_width_,
                     item_count_per_dim_.z * item_width_, channel_count);
    texture_ = gl::Texture3D::create(
        pixels_.getWidth(), pixels_.getHeight(), pixels_.getDepth(),
        gl::getGLInternalFormat<unsigned char>(pixels_.getNumChannels()));
  }

  void addTexture(const Leaf& leaf) {
    int x = item_count_ % item_count_per_dim_.x;
    int y = (item_count_ / item_count_per_dim_.x) % item_count_per_dim_.y;
    int z = item_count_ / (item_count_per_dim_.x * item_count_per_dim_.y);

    auto pix = leaf.getPixels(pixels_.getNumChannels());

    pixels_.loadData(&pix.getData()[0], item_width_, item_width_, item_width_,
                     x * item_width_, y * item_width_, z * item_width_);

    ++item_count_;
  }

  void update() { texture_->loadData(&pixels_.getData()[0]); }

  gl::Texture3D::Ptr& getTexture() { return texture_; }
  const gl::Texture3D::Ptr& getTexture() const { return texture_; }

 private:
  gl::Texture3D::Ptr texture_;
  Pixels3D pixels_;
  glm::ivec3 item_count_per_dim_;
  size_t item_count_;
  size_t item_width_;
};
}  // namespace rs