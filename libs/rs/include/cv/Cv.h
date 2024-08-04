#pragma once

#include "graphics/Pixels.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

namespace rs {

template <typename PixelType>
inline cv::Mat toCv(const BasePixels2D<PixelType>& pix) {
  cv::Mat mat(
      pix.getHeight(), pix.getWidth(),
      CV_MAKETYPE(cv::DataDepth<PixelType>::value, pix.getNumChannels()));
  std::memcpy(mat.data, &pix.getData()[0],
              pix.getSize() * pix.getPixelSizeInBytes());
  return mat;
}

template <typename PixelType>
inline void toPixels(const cv::Mat& mat, BasePixels2D<PixelType>& pixels) {
  if (pixels.getWidth() != mat.cols || pixels.getHeight() != mat.rows ||
      pixels.getNumChannels() != mat.channels())
    pixels.allocate(mat.cols, mat.rows, mat.channels());

  pixels.loadData(mat.ptr<PixelType>(), mat.cols, mat.rows);
}

}  // namespace rs
