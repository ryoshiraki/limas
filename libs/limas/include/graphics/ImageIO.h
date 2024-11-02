#pragma once

#include "graphics/Image.h"
#include "system/Exception.h"
#include "system/Logger.h"
#include "tinyexr.h"
#include "utils/FileSystem.h"

namespace limas {

class ImageIO {
 public:
  ImageIO() = delete;

  template <typename PixelType = unsigned char>
  static BasePixels2D<PixelType> loadPixels(const std::string& filepath,
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
    if (data == nullptr) throw Exception("Couldn't load " + filepath);

    if (desired_num_channels > 0) num_channels = desired_num_channels;
    BasePixels2D<PixelType> pixels;
    pixels.allocate(width, height, num_channels);
    pixels.loadData(data);
    stbi_image_free(data);
    return pixels;
  }

  template <typename PixelType = unsigned char>
  static BaseImage<PixelType> load(const std::string& filepath,
                                   int desired_num_channels = 0) {
    BaseImage<PixelType> image;
    BasePixels2D<PixelType> pixels =
        ImageIO::loadPixels<PixelType>(filepath, desired_num_channels);
    image.setFromPixels(pixels);
    return image;
  }

  template <typename PixelType = unsigned char>
  static BasePixels2D<PixelType> loadPixelsFromMemory(
      const PixelType* mem, const unsigned int mem_len) {
    int width, height, num_channels;
    PixelType* data =
        stbi_load_from_memory(mem, mem_len, &width, &height, &num_channels, 0);
    if (data == nullptr) throw Exception("Couldn't load pixels from memery");

    BasePixels2D<PixelType> pixels;
    pixels.allocate(width, height, num_channels);
    pixels.loadData(data);
    stbi_image_free(data);
    return pixels;
  }

  template <typename PixelType = unsigned char>
  static BaseImage<PixelType> loadFromMemory(const PixelType* mem,
                                             const unsigned int mem_len) {
    BaseImage<PixelType> image;
    BasePixels2D<PixelType> pixels = loadPixelsFromMemory(mem, mem_len);
    image.setFromPixels(pixels);
    return image;
  }

  static FloatImage loadEXR(const std::string& filepath) {
    FloatImage image;
    float* out;  // width * height * RGBA
    int width;
    int height;
    const char* err = nullptr;  // or nullptr in C++11

    int ret = LoadEXR(&out, &width, &height, filepath.c_str(), &err);

    if (ret != TINYEXR_SUCCESS) {
      if (err) {
        logger::error("loadExr") << err << logger::end;
        FreeEXRErrorMessage(err);  // release memory of error message.
      }
    } else {
      FloatPixels2D pixels;
      pixels.allocate(width, height, 4);
      pixels.loadData(out);
      image.setFromPixels(pixels);
      free(out);  // release memory of image data
    }

    return image;
  }

  template <typename PixelType>
  static void savePixels(BasePixels2D<PixelType>& pixels,
                         const std::string& filepath) {
    auto ext = fs::getExtension(filepath);
    if (ext.empty())
      throw Exception("filepath have no extension");
    else if (ext == ".png")
      stbi_write_png(filepath.c_str(), pixels.getWidth(), pixels.getHeight(),
                     pixels.getNumChannels(), &pixels.getData()[0], 0);
    else if (ext == ".bmp")
      stbi_write_bmp(filepath.c_str(), pixels.getWidth(), pixels.getHeight(),
                     pixels.getNumChannels(), &pixels.getData()[0]);
    else if (ext == ".tga")
      stbi_write_tga(filepath.c_str(), pixels.getWidth(), pixels.getHeight(),
                     pixels.getNumChannels(), &pixels.getData()[0]);
    else if (ext == ".jpg")
      stbi_write_jpg(filepath.c_str(), pixels.getWidth(), pixels.getHeight(),
                     pixels.getNumChannels(), &pixels.getData()[0], 0);
    else
      throw Exception(ext + " is not supported");
  }

  template <typename PixelType>
  static void save(BaseImage<PixelType>& image, const std::string& filepath) {
    savePixels(image.getPixels(), filepath);
  }
};

}  // namespace limas