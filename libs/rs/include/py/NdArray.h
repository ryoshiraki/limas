#pragma once
// #define BOOST_PYTHON_STATIC_LIB
#include "boost/python.hpp"
#include "boost/python/numpy.hpp"
#include "graphics/Pixels.h"
#include "opencv2/opencv.hpp"
#include "system/Exception.h"
#include "system/Logger.h"

namespace rs {

namespace py = boost::python;
namespace np = boost::python::numpy;

class NdArray {
  template <typename T>
  struct Conv {
    static T conv(const np::ndarray& arr) {
      np::ndarray row = np::array(arr).astype(np::dtype::get_builtin<T>());
      return reinterpret_cast<T>(row);
    }
  };

#pragma mark Convert to NdArray
  template <>
  struct Conv<NdArray> {
    static NdArray conv(const np::ndarray& arr) { return NdArray(arr); }
  };

#pragma mark Convert to GLM Vector
  template <typename T, glm::length_t L, glm::qualifier Q>
  struct Conv<std::vector<glm::vec<L, T, Q>>> {
    static std::vector<glm::vec<L, T, Q>> conv(const np::ndarray& arr) {
      if (arr.get_nd() != 2 || arr.shape(1) != L) {
        throw rs::Exception("Dimension mismatch");
      }

      T* data = reinterpret_cast<T*>(
          arr.astype(np::dtype::get_builtin<T>()).get_data());

      std::vector<glm::vec<L, T, Q>> result(arr.shape(0));
      for (int i = 0; i < arr.shape(0); ++i) {
        for (glm::length_t j = 0; j < L; ++j) {
          result[i][j] = data[L * i + j];
        }
      }

      return result;
    }
  };

#pragma mark Convert to std::vector
  template <typename T>
  struct Conv<std::vector<T>> {
    static std::vector<T> conv(const np::ndarray& arr) {
      if (arr.get_nd() != 1) {
        throw rs::Exception("Dimension mismatch");
      }

      T* data = reinterpret_cast<T*>(
          arr.astype(np::dtype::get_builtin<T>()).get_data());
      std::vector<T> result(data, data + arr.shape(0));

      return result;
    }
  };

#pragma mark Convert to 2d std::vector
  template <typename T>
  struct Conv<std::vector<std::vector<T>>> {
    static std::vector<std::vector<T>> conv(const np::ndarray& arr) {
      std::vector<std::vector<T>> result(arr.shape(0));

      for (int i = 0; i < result.size(); ++i) {
        result[i] = Conv<std::vector<T>>::conv(np::array(arr[i]));
      }

      return result;
    }
  };

#pragma mark Convert to 3d std::vector
  template <typename T>
  struct Conv<std::vector<std::vector<std::vector<T>>>> {
    static std::vector<std::vector<std::vector<T>>> conv(
        const np::ndarray& arr) {
      std::vector<std::vector<std::vector<T>>> result(arr.shape(0));

      for (int i = 0; i < result.size(); ++i) {
        result[i] = Conv<std::vector<std::vector<T>>>::conv(np::array(arr[i]));
      }

      return result;
    }
  };

#pragma mark Convert to cv::Mat
  template <>
  struct Conv<cv::Mat> {
    static cv::Mat conv(const np::ndarray& arr) {
      if (arr.get_nd() != 3) {
        throw rs::Exception("Dimension mismatch");
      }

      int type = -1;
      if (arr.get_dtype() == np::dtype::get_builtin<float>()) {
        type = CV_32F;
      } else if (arr.get_dtype() == np::dtype::get_builtin<double>()) {
        type = CV_64F;
      } else if (arr.get_dtype() == np::dtype::get_builtin<int>()) {
        type = CV_32S;
      } else {
        throw rs::Exception("Type mismatch: Incompatible type for cv::Mat");
      }

      cv::Mat mat(arr.shape(0), arr.shape(1), type,
                  reinterpret_cast<void*>(arr.get_data()));

      // The OpenCV Mat will take ownership of the data. If the numpy array gets
      // garbage collected, this data will not be deleted.
      mat.addref();

      return mat;
    }
  };

#pragma mark Convert to Pixels
  template <typename T>
  struct Conv<BasePixels2D<T>> {
    static BasePixels2D<T> conv(const np::ndarray& arr) {
      if (arr.get_nd() != 3) {
        throw rs::Exception("Dimension mismatch");
      }

      BasePixels2D<T> pixels(arr.shape(1), arr.shape(0), arr.shape(2));
      pixels.loadData(reinterpret_cast<T*>(arr.get_data()), arr.shape(1),
                      arr.shape(0));
      return pixels;
    }
  };

 public:
  // NdArray() = delete;
  NdArray()
      : arr_(np::zeros(py::make_tuple(0), np::dtype::get_builtin<float>())) {}
  NdArray(const np::ndarray& arr) : arr_(arr) {}

  template <typename T>
  T convert() const {
    T result;
    try {
      result = Conv<T>::conv(arr_);
      return result;
    } catch (const rs::Exception& e) {
      log::warn(e.what());
    } catch (py::error_already_set const&) {
      PyErr_Print();
    }
    return result;
  }

  np::ndarray& get() { return arr_; }
  const np::ndarray& get() const { return arr_; }

  template <typename T>
  static np::ndarray createWithShape(size_t width, size_t height = 0,
                                     size_t depth = 0) {
    py::tuple shape = depth > 0 ? py::make_tuple(width, height, depth)
                                : (height > 0 ? py::make_tuple(width, height)
                                              : py::make_tuple(width));
    np::dtype dtype = np::dtype::get_builtin<T>();
    np::ndarray arr = np::zeros(shape, dtype);
    return arr;
  }

  template <typename PixelType>
  static np::ndarray createFrom(BasePixels3D<PixelType>& pixels) {
    py::tuple shape =
        pixels.getDepth() > 1
            ? py::make_tuple(pixels.getWidth(), pixels.getHeight(),
                             pixels.getDepth(), pixels.getNumChannels())
            : (pixels.getHeight() > 1
                   ? py::make_tuple(pixels.getWidth(), pixels.getHeight(),
                                    pixels.getNumChannels())
                   : py::make_tuple(pixels.getWidth(),
                                    pixels.getNumChannels()));

    np::dtype dtype = np::dtype::get_builtin<PixelType>();
    np::ndarray arr = np::zeros(shape, dtype);
    std::copy(pixels.getData().begin(), pixels.getData().end(),
              reinterpret_cast<PixelType*>(arr.get_data()));
    return arr;
  }

  template <typename T>
  static np::ndarray createFrom(const std::vector<T>& vec) {
    np::dtype dt = np::dtype::get_builtin<T>();
    py::tuple shape = py::make_tuple(vec.size());
    np::ndarray arr = np::zeros(shape, dt);
    std::copy(vec.begin(), vec.end(), reinterpret_cast<T*>(arr.get_data()));
    return arr;
  }

  template <typename T>
  static np::ndarray createFrom(const std::vector<std::vector<T>>& vec) {
    np::dtype dt = np::dtype::get_builtin<T>();
    py::tuple shape = py::make_tuple(vec.size(), vec[0].size());
    np::ndarray arr = np::zeros(shape, dt);
    for (size_t i = 0; i < vec.size(); ++i) {
      for (size_t j = 0; j < vec[0].size(); ++j) {
        arr[i][j] = vec[i][j];
      }
    }
    return arr;
  }

 private:
  np::ndarray arr_;
};

}  // namespace rs
