#pragma once

#include "graphics/Color.h"
#include "system/Exception.h"

namespace limas {

template <typename PixelType>
class BasePixels3D {
 public:
  BasePixels3D() : width_(0), height_(0), depth_(0), channels_(0) {}
  BasePixels3D(size_t width, size_t height, size_t depth, size_t channels) {
    allocate(width, height, depth, channels);
  }

  void allocate(size_t width, size_t height, size_t depth, size_t channels) {
    width_ = width;
    height_ = height;
    depth_ = depth;
    channels_ = channels;
    data_.resize(width_ * height_ * depth_ * channels_);
  }

  void loadData(const PixelType* data, size_t width = 0, size_t height = 0,
                size_t depth = 0, size_t offset_x = 0, size_t offset_y = 0,
                size_t offset_z = 0) {
    width = width == 0 ? width_ : width;
    height = height == 0 ? height_ : height;
    depth = depth == 0 ? depth_ : depth;

    size_t data_index = 0;
    for (size_t dz = 0; dz < depth; ++dz) {
      for (size_t dy = 0; dy < height; ++dy) {
        for (size_t dx = 0; dx < width; ++dx) {
          int idx = getIndex(offset_x + dx, offset_y + dy, offset_z + dz);
          for (size_t c = 0; c < channels_; ++c) {
            data_[idx + c] = data[data_index++];
          }
        }
      }
    }
  }

  std::vector<PixelType>& getData() { return data_; }
  const std::vector<PixelType>& getData() const { return data_; }

  void clear() {
    for (auto& d : data_) {
      d = 0;
    }
  }

  void setColor(const BaseColor<PixelType>& col, size_t x, size_t y, size_t z) {
    int idx = getIndex(x, y, z);
    for (int c = 0; c < channels_; ++c) {
      data_[idx + c] = col[c];
    }
  }

  BaseColor<PixelType> getColor(size_t x, size_t y, size_t z) const {
    int idx = getIndex(x, y, z);
    BaseColor<PixelType> col;
    for (int c = 0; c < channels_; ++c) {
      col[c] = data_[idx + c];
    }

    if (channels_ < 4) col[3] = 1;

    return col;
  }

  BaseColor<PixelType> getColorInterpolated(float fx, float fy,
                                            float fz) const {
    int x = floor(fx);
    int y = floor(fy);
    int z = floor(fz);

    int x1 = x + 1;
    int y1 = y + 1;
    int z1 = z + 1;

    if (x1 >= width_) x1 = x;
    if (y1 >= height_) y1 = y;
    if (z1 >= depth_) z1 = z;

    auto c000 = getColor(x, y, z);
    auto c001 = getColor(x, y, z1);
    auto c010 = getColor(x, y1, z);
    auto c011 = getColor(x, y1, z1);
    auto c100 = getColor(x1, y, z);
    auto c101 = getColor(x1, y, z1);
    auto c110 = getColor(x1, y1, z);
    auto c111 = getColor(x1, y1, z1);

    float xd = fx - x;
    float yd = fy - y;
    float zd = fz - z;

    auto c00 = c000 * (1 - xd) + c100 * xd;
    auto c01 = c001 * (1 - xd) + c101 * xd;
    auto c10 = c010 * (1 - xd) + c110 * xd;
    auto c11 = c011 * (1 - xd) + c111 * xd;

    auto c0 = c00 * (1 - yd) + c10 * yd;
    auto c1 = c01 * (1 - yd) + c11 * yd;

    auto c = c0 * (1 - zd) + c1 * zd;
    return c;
  }

  BasePixels3D<PixelType> getCropped(size_t x, size_t y, size_t z, size_t width,
                                     size_t height, size_t depth) const {
    BasePixels3D<PixelType> output(width, height, depth, channels_);
    auto data = crop(x, y, z, width, height, depth);
    output.loadData(&data[0]);
    return output;
  }

  BasePixels3D<PixelType> remapChannels(
      const std::vector<size_t>& channel_map) {
    BasePixels3D<PixelType> output(width_, height_, depth_, channel_map.size());
    auto data = remap(channel_map);
    output.loadData(&data[0]);
    return output;
  }

  size_t getWidth() const { return width_; }
  size_t getHeight() const { return height_; }
  size_t getDepth() const { return depth_; }
  size_t getSize() const { return width_ * height_ * depth_ * channels_; }
  size_t getNumChannels() const { return channels_; }
  size_t getPixelSizeInBytes() const { return sizeof(PixelType); }
  size_t getIndex(size_t x, size_t y, size_t z) const {
    return channels_ *
           size_t(x + size_t(width_ * y) + size_t(width_ * height_ * z));
  }

 protected:
  std::vector<PixelType> crop(size_t x, size_t y, size_t z, size_t width,
                              size_t height, size_t depth) const {
    std::vector<PixelType> data(width * height * depth * channels_);
    for (size_t dz = 0; dz < depth; ++dz) {
      for (int dy = 0; dy < height; ++dy) {
        for (int dx = 0; dx < width; ++dx) {
          size_t src_x = x + dx;
          size_t src_y = y + dy;
          size_t src_z = z + dz;

          if (src_x >= width_ || src_y >= height_ || src_z >= depth_) {
            throw limas::Exception("Crop region exceeds image bounds.");
          }

          size_t src_idx = getIndex(src_x, src_y, src_z);

          size_t dst_idx = channels_ * (dx + width * (dy + height * dz));

          for (size_t c = 0; c < channels_; ++c) {
            data[dst_idx + c] = data_[src_idx + c];
          }
        }
      }
    }
    return data;
  }

  std::vector<PixelType> remap(const std::vector<size_t>& channel_map) {
    std::vector<PixelType> data(width_ * height_ * depth_ * channel_map.size());
    for (size_t dz = 0; dz < depth_; ++dz) {
      for (size_t dy = 0; dy < height_; ++dy) {
        for (size_t dx = 0; dx < width_; ++dx) {
          int src_idx = getIndex(dx, dy, dz);
          int dst_idx = channel_map.size() * size_t(dx + size_t(width_ * dy));
          for (size_t c = 0; c < channel_map.size(); ++c) {
            data[dst_idx + c] = data_[src_idx + channel_map[c]];
          }
        }
      }
    }
    return data;
  }

  size_t width_, height_, depth_;
  size_t channels_;
  std::vector<PixelType> data_;
};

template <typename PixelType>
class BasePixels2D : public BasePixels3D<PixelType> {
 public:
  BasePixels2D() : BasePixels3D<PixelType>() {}
  BasePixels2D(size_t width, size_t height, size_t channels)
      : BasePixels3D<PixelType>(width, height, 1, channels) {}

  BasePixels2D(const BasePixels3D<PixelType>& pixels3D)
      : BasePixels3D<PixelType>(pixels3D) {
    if (pixels3D.getDepth() != 1) {
      throw limas::Exception("Cannot convert 3D pixels to 2D: depth is not 1.");
    }
  }

  void allocate(size_t width, size_t height, size_t channels) {
    BasePixels3D<PixelType>::allocate(width, height, 1, channels);
  }

  void loadData(const PixelType* data, size_t width = 0, size_t height = 0,
                size_t offset_x = 0, size_t offset_y = 0) {
    BasePixels3D<PixelType>::loadData(data, width, height, 1, offset_x,
                                      offset_y, 0);
  }

  void setColor(const BaseColor<PixelType>& col, size_t x, size_t y) {
    BasePixels3D<PixelType>::setColor(col, x, y, 0);
  }
  BaseColor<PixelType> getColor(size_t x, size_t y) const {
    return BasePixels3D<PixelType>::getColor(x, y, 0);
  }

  BaseColor<PixelType> getColorInterpolated(float fx, float fy) const {
    int x = floor(fx);
    int y = floor(fy);

    int x1 = x + 1;
    int y1 = y + 1;

    if (x1 >= this->width_) x1 = x;
    if (y1 >= this->height_) y1 = y;

    auto c00 = BasePixels3D<PixelType>::getColor(x, y, 0);
    auto c01 = BasePixels3D<PixelType>::getColor(x, y1, 0);
    auto c10 = BasePixels3D<PixelType>::getColor(x1, y, 0);
    auto c11 = BasePixels3D<PixelType>::getColor(x1, y1, 0);

    float xd = fx - x;
    float yd = fy - y;

    auto c0 = c00 * (1 - xd) + c10 * xd;
    auto c1 = c01 * (1 - xd) + c11 * xd;

    auto c = c0 * (1 - yd) + c1 * yd;

    return c;
  }

  BasePixels2D<PixelType> getCropped(size_t x, size_t y, size_t width,
                                     size_t height) const {
    return BasePixels3D<PixelType>::getCropped(x, y, 0, width, height, 1);
  }

  BasePixels2D<PixelType> remapChannels(
      const std::vector<size_t>& channel_map) {
    BasePixels2D<PixelType> output(this->width_, this->height_,
                                   channel_map.size());
    auto data = this->remap(channel_map);
    output.loadData(&data[0]);
    return output;
  }

  size_t getIndex(size_t x, size_t y) const {
    return BasePixels3D<PixelType>::getIndex(x, y, 0);
  }
};

template <typename PixelType>
class BasePixels1D : public BasePixels2D<PixelType> {
 public:
  BasePixels1D() : BasePixels2D<PixelType>() {}
  BasePixels1D(size_t width, size_t channels)
      : BasePixels2D<PixelType>(width, 1, channels) {}

  BasePixels1D(const BasePixels2D<PixelType>& pixels2D)
      : BasePixels2D<PixelType>(pixels2D) {
    if (pixels2D.getHeight() != 1) {
      throw limas::Exception(
          "Cannot convert 2D pixels to 1D: height is not 1.");
    }
  }

  void allocate(size_t width, size_t channels) {
    BasePixels2D<PixelType>::allocate(width, 1, channels);
  }

  void loadData(const PixelType* data, size_t width = 0, size_t offset_x = 0) {
    BasePixels3D<PixelType>::loadData(data, width, 1, 1, offset_x, 0, 0);
  }

  void setColor(const BaseColor<PixelType>& col, size_t x) {
    BasePixels2D<PixelType>::setColor(col, x, 0);
  }
  BaseColor<PixelType> getColor(size_t x) const {
    return BasePixels2D<PixelType>::getColor(x, 0);
  }

  BaseColor<PixelType> getColorInterpolated(float fx) const {
    int x = floor(fx);
    int x1 = x + 1;

    if (x1 >= this->width_) x1 = x;

    auto c0 = BasePixels2D<PixelType>::getColor(x, 0);
    auto c1 = BasePixels2D<PixelType>::getColor(x1, 0);

    float xd = fx - x;
    auto c = c0 * (1 - xd) + c1 * xd;

    return c;
  }

  BasePixels1D<PixelType> getCropped(size_t x, size_t width) const {
    return BasePixels2D<PixelType>::getCropped(x, 0, 0, width, 1, 1);
  }

  BasePixels1D<PixelType> remapChannels(
      const std::vector<size_t>& channel_map) {
    BasePixels1D<PixelType> output(this->width_, channel_map.size());
    auto data = this->remap(channel_map);
    output.loadData(&data[0]);
    return output;
  }

  size_t getIndex(size_t x) const {
    return BasePixels2D<PixelType>::getIndex(x, 0, 0);
  }
};

using Pixels1D = BasePixels1D<unsigned char>;
using ShortPixels1D = BasePixels1D<short>;
using IntPixels1D = BasePixels1D<int>;
using FloatPixels1D = BasePixels1D<float>;

using Pixels2D = BasePixels2D<unsigned char>;
using ShortPixels2D = BasePixels2D<short>;
using IntPixels2D = BasePixels2D<int>;
using FloatPixels2D = BasePixels2D<float>;

using Pixels3D = BasePixels3D<unsigned char>;
using ShortPixels3D = BasePixels3D<short>;
using IntPixels3D = BasePixels3D<int>;
using FloatPixels3D = BasePixels3D<float>;

}  // namespace limas