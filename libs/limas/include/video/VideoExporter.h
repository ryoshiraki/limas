#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
// #include <libavutil/imgutils.h>
// #include <libavutil/pixdesc.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#include "gl/Fbo.h"
#include "gl/Pbo.h"
#include "gl/Shader.h"
#include "gl/Texture2D.h"
#include "gl/VboMesh.h"
#include "graphics/ImageIO.h"
#include "primitives/Primitives.h"
#include "system/Logger.h"
#include "utils/Stopwatch.h"

namespace limas {
class VideoExporter {
#define ENCODE_HEVC 0
  gl::Fbo fbo_;
  gl::PboPacker pbo_;
  std::vector<unsigned char> pixel_data_;

  struct Context {
    AVStream *stream;
    AVFormatContext *format_context;
    AVCodecContext *codec_context;
    AVFrame *frame;
    AVFrame *yuv_frame;
    SwsContext *sws_context;
    uint32_t frame_index;
  };
  Context context_;

  bool is_setup_;
  bool is_exporting_;
  bool b_vflip_;

 public:
  VideoExporter() : is_setup_(false), is_exporting_(false), b_vflip_(true) {}
  ~VideoExporter() { stop(); }

  bool allocate(size_t width, size_t height, float fps) {
    stop();

    do {
      fbo_.allocate(width, height);
      fbo_.attachColor(GL_RGBA8);
      fbo_.attachDepth();
      fbo_.getTexture(0).setMinFilter(GL_NEAREST);
      fbo_.getTexture(0).setMagFilter(GL_NEAREST);

      pbo_.allocate(fbo_.getWidth(), fbo_.getHeight(),
                    fbo_.getTextures()[0].getInternalFormat());
      // pbo_->enableAsync();

      pixel_data_.resize(width * height * 4);

      avformat_alloc_output_context2(&context_.format_context, nullptr, "mov",
                                     nullptr);  // filepath.c_str());
      if (!context_.format_context) {
        logger::error("VideoExporter")
            << "Couldn't allocate format context" << logger::end();
        break;
      }

      const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_PRORES);
      if (!codec) {
        logger::error("VideoExporter")
            << "Couldn't find condec" << logger::end();
        break;
      }

      if (!(context_.stream =
                avformat_new_stream(context_.format_context, nullptr))) {
        logger::error("VideoExporter")
            << "Couldn't create strean" << logger::end();
        return false;
      }
      context_.stream->id = (int)(context_.format_context->nb_streams - 1);
      context_.stream->time_base = av_d2q(1.0 / fps, 120);

      if (!(context_.codec_context = avcodec_alloc_context3(codec))) {
        logger::error("VideoExporter")
            << "Couldn't allocate condec context" << logger::end();
        break;
      }
      context_.codec_context->width = width;
      context_.codec_context->height = height;
      context_.codec_context->time_base = context_.stream->time_base;
      context_.codec_context->pix_fmt = AV_PIX_FMT_YUV444P10;
      context_.codec_context->codec_id = AV_CODEC_ID_PRORES;
      context_.codec_context->profile = FF_PROFILE_PRORES_4444;

      // generate global header when the format requires it
      if (context_.format_context->oformat->flags & AVFMT_GLOBALHEADER) {
        context_.format_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
      }

      AVDictionary *codec_options = nullptr;
      //   av_dict_set(&codec_options, "codec", "prores_ks4", 0);

      if (avcodec_open2(context_.codec_context, codec, &codec_options) < 0) {
        logger::error("VideoExporter")
            << "Couldn't open codec" << logger::end();
        break;
      }

      if (!(context_.frame = av_frame_alloc())) {
        logger::error("VideoExporter")
            << "Couldn't allocate frame" << logger::end();
        break;
      }
      context_.frame->format = context_.codec_context->pix_fmt;
      context_.frame->width = context_.codec_context->width;
      context_.frame->height = context_.codec_context->height;

      if (av_frame_get_buffer(context_.frame, 32) < 0) {
        logger::error("VideoExporter")
            << "Couldn't allocate frame data" << logger::end();
        break;
      }

      if (avcodec_parameters_from_context(context_.stream->codecpar,
                                          context_.codec_context) < 0) {
        logger::error("VideoExporter")
            << "Couldn't copy the stream parameter" << logger::end();
        break;
      }

      if (!(context_.sws_context = sws_getContext(
                context_.codec_context->width, context_.codec_context->height,
                AV_PIX_FMT_RGBA, context_.codec_context->width,
                context_.codec_context->height, context_.codec_context->pix_fmt,
                SWS_BILINEAR, nullptr, nullptr, nullptr))) {
        logger::error("VideoExporter")
            << "Couldn't get sws context" << logger::end();
        break;
      }

      // if (avio_open(&context_.format_context->pb, filepath.c_str(),
      //               AVIO_FLAG_WRITE) != 0) {
      //   logger::error("VideoExporter")
      //       << "Couldn't open " << filepath << logger::end();
      //   break;
      // }

      // if (avformat_write_header(context_.format_context, nullptr) < 0) {
      //   logger::error("VideoExporter") << "Couldn't write" << logger::end();
      //   if (avio_close(context_.format_context->pb) != 0)
      //     logger::error("VideoExporter")
      //         << "Couldn't close file" << logger::end();
      //   break;
      // }

      // av_dump_format(context_.format_context, 0, filepath.c_str(), 1);

      context_.frame_index = 0;
      is_setup_ = true;
      is_exporting_ = false;
      return true;
    } while (false);

    stop();
    return false;
  }

  void stop() {
    if (is_setup_) {
      avcodec_send_frame(context_.codec_context, nullptr);

      flush();

      av_write_trailer(context_.format_context);

      if (avio_close(context_.format_context->pb) != 0)
        logger::error("VideoExporter")
            << "failed to close file" << logger::end();
    }

    if (context_.sws_context) sws_freeContext(context_.sws_context);
    if (context_.frame) av_frame_free(&context_.frame);
    if (context_.codec_context) avcodec_free_context(&context_.codec_context);
    if (context_.format_context) avformat_free_context(context_.format_context);

    is_setup_ = false;
    is_exporting_ = false;
  }

  void bind() { fbo_.bind(); }
  void unbind() {
    fbo_.unbind();

    if (is_setup_ && is_exporting_ && pbo_.readToPixels(&pixel_data_, fbo_)) {
      write(pixel_data_.data());
    }
  }

  void write(const unsigned char *data) {
    auto ret = av_frame_make_writable(context_.frame);
    if (ret < 0) {
      std::cout << "frame is not writable" << std::endl;
      return;
    }

    // flip vertically
    if (b_vflip_) {
      const int width = context_.codec_context->width;
      const int height = context_.codec_context->height;
      const int bytes_per_pixel = 4;
      const unsigned char *flipped_data =
          data + (height - 1) * width * bytes_per_pixel;

      const int flipped_linesize[1] = {-width * bytes_per_pixel};

      sws_scale(context_.sws_context, &flipped_data, flipped_linesize, 0,
                height, context_.frame->data, context_.frame->linesize);
    } else {
      const int in_linesize[1] = {context_.codec_context->width * 4};
      sws_scale(context_.sws_context, &data, in_linesize, 0,
                context_.codec_context->height, context_.frame->data,
                context_.frame->linesize);
    }

    context_.frame->pts = context_.frame_index++;
    ret = avcodec_send_frame(context_.codec_context, context_.frame);
    if (ret < 0) {
      logger::error("VideoExporter")
          << "Error sending a frame for encoding" << logger::end();
      return;
    }

    flush();
  }

  void start(const std::string &filepath) {
    if (avio_open(&context_.format_context->pb, filepath.c_str(),
                  AVIO_FLAG_WRITE) != 0) {
      logger::error("VideoExporter")
          << "Couldn't open " << filepath << logger::end();
      return;
    }

    if (avformat_write_header(context_.format_context, nullptr) < 0) {
      logger::error("VideoExporter") << "Couldn't write" << logger::end();
      avio_close(context_.format_context->pb);
      return;
    }

    av_dump_format(context_.format_context, 0, filepath.c_str(), 1);

    context_.frame_index = 0;
    is_exporting_ = true;
  }

  uint32_t getRecordedFrameCount() const { return context_.frame_index; }

  void setVFlip(bool b_vflip) { b_vflip_ = b_vflip; }
  bool isExporting() const { return is_exporting_; }
  bool isVFlip() const { return b_vflip_; }
  size_t getWidth() const { return fbo_.getWidth(); }
  size_t getHeight() const { return fbo_.getHeight(); }
  glm::vec2 getSize() const {
    return glm::vec2(fbo_.getWidth(), fbo_.getHeight());
  }
  const gl::Texture2D &getTexture() const { return fbo_.getTexture(0); }

  bool saveCurrentFrame(const std::string &path) {
    if (!is_setup_) {
      logger::error("VideoExporter")
          << "VideoExporter is not set up" << logger::end();
      return false;
    }

    Pixels2D pixels;
    pixels.allocate(fbo_.getWidth(), fbo_.getHeight(), 4);

    if (!pbo_.readToPixels(&pixels.getData(), fbo_)) {
      logger::error("VideoExporter")
          << "Failed to read pixels from FBO" << logger::end();
      return false;
    }

    // pixels.flip(false, true);
    ImageIO::savePixels(path, pixels);
    return true;
  }

 private:
  bool flush() {
    int ret;
    do {
      AVPacket packet = {0};

      ret = avcodec_receive_packet(context_.codec_context, &packet);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;

      if (ret < 0) {
        logger::error("VideoExporter")
            << "Error encoding a frame" << logger::end();
        return false;
      }

      av_packet_rescale_ts(&packet, context_.codec_context->time_base,
                           context_.stream->time_base);
      packet.stream_index = context_.stream->index;

      ret = av_interleaved_write_frame(context_.format_context, &packet);
      av_packet_unref(&packet);
      if (ret < 0) {
        logger::error("VideoExporter")
            << "Error while writing output packet" << logger::end();
        return false;
      }
    } while (ret >= 0);

    return true;
  }
};
}  // namespace limas