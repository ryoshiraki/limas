#pragma once

#include "Processing.NDI.Lib.h"
#include "gl/Fbo.h"
#include "gl/Pbo.h"
#include "system/Logger.h"
#include "utils/PingPong.h"

namespace limas {

template <typename T>
class BaseNDISender {
 public:
  ~BaseNDISender() { NDIlib_send_destroy(ndi_send_); }

  void setup(const std::string& name, size_t width, size_t height) {
    if (!NDIlib_initialize()) {
      log::error("NDI Sender") << "Couldn't initialize NDIlib" << log::end();
      return;
    }

    NDIlib_send_create_t send_desc;
    send_desc.p_ndi_name = name.c_str();

    ndi_send_ = NDIlib_send_create(&send_desc);
    if (!ndi_send_) {
      log::error("NDI Sender")
          << "Couldn't create NDIlib_send_instance_t" << log::end();
      return;
    }

    GLenum internal_format = GL_RGBA8;
    size_t stride = 4;
    switch (fourCC_type_) {
      case NDIlib_FourCC_video_type_RGBA: {
        internal_format = GL_RGBA8;
        stride = 4;
        break;
      }
      case NDIlib_FourCC_video_type_P216: {
        internal_format = GL_RGB16;
        stride = 2;
        break;
      }
      default:
        break;
    }

    size_t channels = gl::getNumChannelsFromInternal(internal_format);

    ndi_video_frame_.xres = width;
    ndi_video_frame_.yres = height;
    ndi_video_frame_.picture_aspect_ratio = (float)width / (float)height;
    ndi_video_frame_.frame_rate_N = 60000;
    ndi_video_frame_.frame_rate_D = 1000;

    ndi_video_frame_.FourCC = fourCC_type_;
    ndi_video_frame_.line_stride_in_bytes = ndi_video_frame_.xres * stride;
    ndi_video_frame_.frame_format_type = NDIlib_frame_format_type_progressive;

    fbo_.allocate(width, height);
    fbo_.attachColor(internal_format);
    fbo_.attachDepth();
    fbo_.getTexture(0).setMinFilter(GL_NEAREST);
    fbo_.getTexture(0).setMagFilter(GL_NEAREST);

    pbo_.allocate(width, height, internal_format);
    for (int i = 0; i < 2; i++) data_[i].resize(width * height * channels);
  }

  void bind() { fbo_.bind(); }
  void unbind() { fbo_.unbind(); }

  void send() {
    if (pbo_.readTo(&data_.getFront(), fbo_.getID(), 0)) {
      send(data_.getFront().data());
      if (pbo_.isAsync()) data_.swap();
    }
  }

  void send(T* data) {
    if (NDIlib_send_get_no_connections(ndi_send_, 0)) {
      ndi_video_frame_.p_data = (uint8_t*)data;
      if (pbo_->isAsync()) {
        NDIlib_send_send_video_async_v2(ndi_send_, &ndi_video_frame_);
      } else
        NDIlib_send_send_video_v2(ndi_send_, &ndi_video_frame_);
    }
  }

  void enableAsync() { pbo_.enableAsync(); }
  void disableAsync() { pbo_.disableAsync(); }

  size_t getWidth() const { return fbo_.getWidth(); }
  size_t getHeight() const { return fbo_.getHeight(); }
  gl::Texture2D::Ptr& getTexture() { return fbo_.getTexture(0); }

 protected:
  BaseNDISender(NDIlib_FourCC_type_e fourCC_type) : fourCC_type_(fourCC_type) {}

  NDIlib_send_instance_t ndi_send_;
  NDIlib_video_frame_v2_t ndi_video_frame_;
  gl::Fbo fbo_;
  gl::PboPacker pbo_;
  PingPong<std::vector<T>> data_;
  const NDIlib_FourCC_type_e fourCC_type_;
};

class NDISender : public BaseNDISender<uint8_t> {
 public:
  NDISender() : BaseNDISender<uint8_t>(NDIlib_FourCC_video_type_RGBA) {}
};

}  // namespace limas
