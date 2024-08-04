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

  inline BaseImage& operator=(const BaseImage& rhs) {
    BaseImage lhs(rhs);
    swap(*this, lhs);
    return *this;
  }

  void allocate(int width, int height) {
    pixels_.allocate(width, height);
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

  void setFromPixels(std::vector<PixelType>& pixels, int width, int height,
                     int channels) {
    pixels_.allocate(width, height, channels);
    tex_.allocate(width, height,
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

  void update() { tex_.loadData(&pixels_.getData()[0]); }

  size_t getWidth() const { return tex_.getWidth(); }
  size_t getHeight() const { return tex_.getHeight(); }
  glm::vec2 getSize() const { return glm::vec2(getWidth(), getHeight()); }
  GLenum getInternalFormat() const { return tex_.getInternalFormat(); }
  size_t getNumChannels() const { return tex_.getNumChannels(); }
  gl::Texture2D& getTexture() { return tex_; }
  BasePixels2D<PixelType>& getPixels() { return pixels_; }

  friend void swap(BaseImage<PixelType>& first, BaseImage<PixelType>& second) {
    using std::swap;
    swap(first.tex_, second.tex_);
    swap(first.pixels_, second.pixels_);
  }

  static bool loadPixels(BasePixels2D<PixelType>* pixels,
                         const std::string& filepath,
                         int desired_num_channels = 0) {
    int width, height, num_channels;
    PixelType* data;
    if constexpr (std::is_same_v<PixelType, float>) {
      data = reinterpret_cast<PixelType*>(stbi_loadf(filepath.c_str(), &width,
                                                     &height, &num_channels,
                                                     desired_num_channels));
    } else {
      data = reinterpret_cast<PixelType*>(stbi_load(filepath.c_str(), &width,
                                                    &height, &num_channels,
                                                    desired_num_channels));
    }
    if (data == nullptr) return false;

    if (desired_num_channels > 0) num_channels = desired_num_channels;
    pixels->allocate(width, height, num_channels);
    pixels->loadData(data);
    stbi_image_free(data);
    return true;
  }

  static bool loadPixelsFromMemory(BasePixels2D<PixelType>* pixels,
                                   const PixelType* mem,
                                   const unsigned int mem_len) {
    int width, height, num_channels;
    PixelType* data =
        stbi_load_from_memory(mem, mem_len, &width, &height, &num_channels, 0);
    if (data == nullptr) return false;
    pixels->allocate(width, height, num_channels);
    pixels->loadData(data);
    stbi_image_free(data);
    return true;
  }

  static bool savePixels(BasePixels2D<PixelType>& pixels,
                         const std::string& filepath) {
    auto ext = fs::getExtension(filepath);
    if (ext == ".png")
      return stbi_write_png(filepath.c_str(), pixels.getWidth(),
                            pixels.getHeight(), pixels.getNumChannels(),
                            &pixels.getData()[0], 0);
    else if (ext == ".bmp")
      return stbi_write_bmp(filepath.c_str(), pixels.getWidth(),
                            pixels.getHeight(), pixels.getNumChannels(),
                            &pixels.getData()[0]);
    else if (ext == ".tga")
      return stbi_write_tga(filepath.c_str(), pixels.getWidth(),
                            pixels.getHeight(), pixels.getNumChannels(),
                            &pixels.getData()[0]);
    else if (ext == ".jpg")
      return stbi_write_jpg(filepath.c_str(), pixels.getWidth(),
                            pixels.getHeight(), pixels.getNumChannels(),
                            &pixels.getData()[0], 0);
    else if (ext.empty()) {
      log::warn() << "filepath have no extension." << log::end();
      return false;
    }
    log::warn() << ext << " is not supported." << log::end();
    return false;
  }

 protected:
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
