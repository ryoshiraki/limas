#pragma once
#include "gl/Texture2D.h"
#include "graphics/Pixels.h"
#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"

namespace limas {

template <typename PixelType>
class BaseImage {
 public:
  BaseImage() {}
  BaseImage(const BaseImage& image) {
    pixels_ = image.pixels_;
    tex_.allocate(pixels_.getWidth(), pixels_.getHeight(),
                  gl::getGLInternalFormat<PixelType>(pixels_.getNumChannels()));
    tex_.loadData(&pixels_.getData()[0]);
  }
  BaseImage(BasePixels2D<PixelType>& pixels) { setFromPixels(pixels); }

  inline BaseImage& operator=(const BaseImage& rhs) {
    BaseImage lhs(rhs);
    swap(*this, lhs);
    return *this;
  }

  void allocate(size_t width, size_t height, size_t num_channels) {
    pixels_.allocate(width, height, num_channels);
    tex_.allocate(pixels_.getWidth(), pixels_.getHeight(),
                  gl::getGLInternalFormat<PixelType>(pixels_.getNumChannels()));
    tex_.loadData(&pixels_.getData()[0]);
  }

  void setFromPixels(BasePixels2D<PixelType>& pixels) {
    pixels_ = pixels;
    tex_.allocate(pixels_.getWidth(), pixels_.getHeight(),
                  gl::getGLInternalFormat<PixelType>(pixels_.getNumChannels()));
    tex_.loadData(&pixels_.getData()[0]);
  }

  void resize(int width, int height) {
    BasePixels2D<PixelType> new_pixels;
    new_pixels.allocate(width, height, getNumChannels());
    stbir_resize_uint8(pixels_.getData(0, 0), getWidth(), getHeight(), 0,
                       new_pixels.getData(0, 0), width, height, 0,
                       static_cast<int>(getNumChannels()));
    setFromPixels(new_pixels);
  }

  void crop(int x, int y, int width, int height) {
    auto new_pixels = pixels_.getCropped(x, y, width, height);
    setFromPixels(new_pixels);
  }

  void flip(bool flip_x, bool flip_y) {
    pixels_.flip(flip_x, flip_y);
    update();
  }

  void update() { tex_.loadData(&pixels_.getData()[0]); }

  size_t getWidth() const { return tex_.getWidth(); }
  size_t getHeight() const { return tex_.getHeight(); }
  glm::vec2 getSize() const { return glm::vec2(getWidth(), getHeight()); }
  GLenum getInternalFormat() const { return tex_.getInternalFormat(); }
  size_t getNumChannels() const { return tex_.getNumChannels(); }
  gl::Texture2D& getTexture() { return tex_; }
  BasePixels2D<PixelType>& getPixels() { return pixels_; }

 protected:
  friend void swap(BaseImage<PixelType>& first, BaseImage<PixelType>& second) {
    using std::swap;
    swap(first.tex_, second.tex_);
    swap(first.pixels_, second.pixels_);
  }

  gl::Texture2D tex_;
  BasePixels2D<PixelType> pixels_;
};

template <typename PixelType>
inline std::ostream& operator<<(std::ostream& os,
                                const BaseImage<PixelType>& image) {
  os << "width:";
  os << image.getWidth();

  os << " height:";
  os << image.getHeight();

  os << " format:";
  os << gl::getStringFromInternal(image.getInternalFormat());

  return os;
}

using Image = BaseImage<unsigned char>;
using ShortImage = BaseImage<short>;
using IntImage = BaseImage<int>;
using FloatImage = BaseImage<float>;

}  // namespace limas
