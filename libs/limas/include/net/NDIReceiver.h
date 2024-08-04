#pragma once

#include "Processing.NDI.Lib.h"
#include "gl/Texture2D.h"
#include "graphics/Pixels.h"
#include "system/Exception.h"
#include "system/Logger.h"
#include "system/Thread.h"

namespace limas {

template <typename T>
class BaseNDIReceiver : public Thread {
 public:
  ~BaseNDIReceiver() { NDIlib_recv_destroy(ndi_receive_); }

  bool connect(const std::string& name, const std::string& ip = "") {
    b_connected_ = false;

    if (!NDIlib_initialize()) {
      log::error("NDI Receiver") << "couldn't initialize NDIlib" << log::end();
      return false;
    }

    NDIlib_find_instance_t ndi_find = NDIlib_find_create_v2();
    do {
      if (!ndi_find) {
        log::error("NDI Receiver")
            << "Couldn't create NDIlib_find_instance_t" << log::end();
        break;
      }

      uint32_t num_sources = 0;
      const NDIlib_source_t* sources = NULL;
      log::info("NDI Receiver") << "Looking for sources ..." << log::end();
      NDIlib_find_wait_for_sources(ndi_find, 5000 /* Five second */);
      sources = NDIlib_find_get_current_sources(ndi_find, &num_sources);

      if (num_sources == 0) {
        log::error("NDI Receiver") << "No source found" << log::end();
        break;
      }

      for (uint32_t i = 0; i < num_sources; i++) {
        log::info("NDI Receiver") << sources[i].p_ndi_name << " "
                                  << sources[i].p_ip_address << log::end();
      }

      NDIlib_recv_create_v3_t recv_desc;
      recv_desc.color_format = color_format_;
      recv_desc.bandwidth = NDIlib_recv_bandwidth_highest;

      ndi_receive_ = NDIlib_recv_create_v3(&recv_desc);
      if (!ndi_receive_) {
        log::error("NDI Receiver")
            << "Couldn't create NDIlib_recv_instance_t" << log::end();
        break;
      }

      for (uint32_t i = 0; i < num_sources; i++) {
        std::string _name = sources[i].p_ndi_name;
        std::string::size_type start = _name.find('(');
        std::string::size_type end = _name.find(')');
        _name = _name.substr(start + 1, end - start - 1);

        std::string _ip = sources[i].p_ip_address;
        end = _ip.find(':');
        _ip = _ip.substr(0, end);

        if (_name == name) {
          if (ip.empty() || ip == _ip) {
            NDIlib_recv_connect(ndi_receive_, sources + i);
            b_connected_ = true;
            break;
          }
        }
      }

    } while (false);

    NDIlib_find_destroy(ndi_find);
    return b_connected_;
  }

  void update() {
    if (!isRunning()) _update();
  }

  bool isFrameNew() {
    auto l = getLock();
    bool tmp = b_data_new_;
    b_data_new_ = false;
    return tmp;
  }

  gl::Texture2D& getTexture() {
    auto l = getLock();
    if (width_ == 0 || height_ == 0) throw rs::Exception("texture size is 0");

    if (!texture_.isAllocated() || texture_.getWidth() != width_ ||
        texture_.getHeight() != height_) {
      texture_.allocate(width_, height_, GL_RGBA8);
    }
    if (b_update_tex_) texture_.loadData(front_data_->data());

    b_update_tex_ = false;
    return texture_;
  }

  size_t getWidth() const {
    auto locker = getLock();
    return width_;
  }

  size_t getHeight() const {
    auto locker = getLock();
    return height_;
  }

  const std::vector<T>& getData() const {
    auto locker = getLock();
    return *front_data_;
  }

  std::vector<T>& getData() {
    auto locker = getLock();
    return *front_data_;
  }

 protected:
  BaseNDIReceiver(NDIlib_recv_color_format_e color_format)
      : color_format_(color_format),
        width_(0),
        height_(0),
        b_connected_(false),
        b_data_new_(false),
        b_update_tex_(false) {
    front_data_ = &data_[0];
    back_data_ = &data_[1];
  }

  NDIlib_recv_instance_t ndi_receive_;
  gl::Texture2D texture_;
  std::vector<T> data_[2];
  std::vector<T>* front_data_;
  std::vector<T>* back_data_;
  size_t width_, height_;
  bool b_data_new_;
  bool b_update_tex_;
  bool b_connected_;
  const NDIlib_recv_color_format_e color_format_;

 private:
  void _update() {
    if (!b_connected_) return;

    NDIlib_video_frame_v2_t video_frame;
    switch (NDIlib_recv_capture_v2(ndi_receive_, &video_frame, nullptr, nullptr,
                                   50)) {
      case NDIlib_frame_type_none:
        log::error("NDI Receiver") << "No data received" << log::end();
        break;

      case NDIlib_frame_type_video: {
        if (width_ != video_frame.xres || height_ != video_frame.yres) {
          width_ = video_frame.xres;
          height_ = video_frame.yres;
          for (int i = 0; i < 2; i++) data_[i].resize(width_ * height_ * 4);
        }

        T* data_received = (uint8_t*)video_frame.p_data;
        std::copy(data_received, data_received + back_data_->size(),
                  back_data_->begin());
        std::swap(front_data_, back_data_);
        b_data_new_ = true;
        b_update_tex_ = true;
        NDIlib_recv_free_video_v2(ndi_receive_, &video_frame);
        break;
      }
      default:
        break;
    }
  }

  void threadedFunction() {
    while (isRunning()) {
      auto locker = getLock();
      _update();
    }
  }
};

class NDIReceiver : public BaseNDIReceiver<uint8_t> {
 public:
  NDIReceiver()
      : BaseNDIReceiver<uint8_t>(NDIlib_recv_color_format_RGBX_RGBA) {}
};

}  // namespace limas
