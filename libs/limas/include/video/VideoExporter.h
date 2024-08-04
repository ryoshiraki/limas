#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
// #include <libavutil/imgutils.h>
// #include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}

#include "gl/Fbo.h"
#include "gl/Pbo.h"
#include "gl/Shader.h"
#include "gl/Texture2D.h"
#include "gl/VboMesh.h"
#include "primitives/Primitives.h"
#include "system/Logger.h"
#include "utils/Stopwatch.h"

namespace limas {
class VideoExporter {
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

 public:
  VideoExporter() : is_setup_(false), is_exporting_(false) {}
  ~VideoExporter() { close(); }

  bool open(size_t width, size_t height, int fps, const std::string &filepath) {
    close();

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

      avformat_alloc_output_context2(&context_.format_context, nullptr, nullptr,
                                     filepath.c_str());
      if (!context_.format_context) {
        log::error("VideoExporter")
            << "Couldn't allocate format context" << log::end();
        break;
      }

      const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_PRORES);
      if (!codec) {
        log::error("VideoExporter") << "Couldn't find condec" << log::end();
        break;
      }

      if (!(context_.stream =
                avformat_new_stream(context_.format_context, nullptr))) {
        log::error("VideoExporter") << "Couldn't create strean" << log::end();
        return false;
      }
      context_.stream->id = (int)(context_.format_context->nb_streams - 1);
      context_.stream->time_base = av_d2q(1.0 / fps, 120);

      if (!(context_.codec_context = avcodec_alloc_context3(codec))) {
        log::error("VideoExporter")
            << "Couldn't allocate condec context" << log::end();
        break;
      }
      context_.codec_context->width = width;
      context_.codec_context->height = height;
      context_.codec_context->time_base = context_.stream->time_base;
      context_.codec_context->pix_fmt = AV_PIX_FMT_YUV444P10;
      context_.codec_context->codec_id = AV_CODEC_ID_PRORES;
      context_.codec_context->profile = FF_PROFILE_PRORES_4444;
      // context_.codec_context->bit_rate = your_desired_bit_rate;
      // context_.codec_context->qmin = your_desired_min_quantizer;
      // context_.codec_context->qmax = your_desired_max_quantizer;

      // generate global header when the format requires it
      if (context_.format_context->oformat->flags & AVFMT_GLOBALHEADER) {
        context_.format_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
      }

      AVDictionary *codec_options = nullptr;
      //   av_dict_set(&codec_options, "codec", "prores_ks4", 0);

      if (avcodec_open2(context_.codec_context, codec, &codec_options) < 0) {
        log::error("VideoExporter") << "Couldn't open codec" << log::end();
        break;
      }

      if (!(context_.frame = av_frame_alloc())) {
        log::error("VideoExporter") << "Couldn't allocate frame" << log::end();
        break;
      }
      context_.frame->format = context_.codec_context->pix_fmt;
      context_.frame->width = context_.codec_context->width;
      context_.frame->height = context_.codec_context->height;

      if (av_frame_get_buffer(context_.frame, 32) < 0) {
        log::error("VideoExporter")
            << "Couldn't allocate frame data" << log::end();
        break;
      }

      if (avcodec_parameters_from_context(context_.stream->codecpar,
                                          context_.codec_context) < 0) {
        log::error("VideoExporter")
            << "Couldn't copy the stream parameter" << log::end();
        break;
      }

      if (!(context_.sws_context = sws_getContext(
                context_.codec_context->width, context_.codec_context->height,
                AV_PIX_FMT_RGBA, context_.codec_context->width,
                context_.codec_context->height, context_.codec_context->pix_fmt,
                SWS_LANCZOS, nullptr, nullptr, nullptr))) {
        log::error("VideoExporter") << "Couldn't get sws context" << log::end();
        break;
      }

      if (avio_open(&context_.format_context->pb, filepath.c_str(),
                    AVIO_FLAG_WRITE) != 0) {
        log::error("VideoExporter")
            << "Couldn't open " << filepath << log::end();
        break;
      }

      if (avformat_write_header(context_.format_context, nullptr) < 0) {
        log::error("VideoExporter") << "Couldn't write" << log::end();
        if (avio_close(context_.format_context->pb) != 0)
          log::error("VideoExporter") << "Couldn't close file" << log::end();
        break;
      }

      av_dump_format(context_.format_context, 0, filepath.c_str(), 1);

      context_.frame_index = 0;
      is_setup_ = true;
      is_exporting_ = false;
      return true;
    } while (false);

    close();
    return false;
  }

  void close() {
    if (is_setup_) {
      avcodec_send_frame(context_.codec_context, nullptr);

      flush();

      av_write_trailer(context_.format_context);

      if (avio_close(context_.format_context->pb) != 0)
        log::error("VideoExporter") << "failed to close file" << log::end();
    }

    if (context_.sws_context) sws_freeContext(context_.sws_context);
    if (context_.frame) av_frame_free(&context_.frame);
    if (context_.codec_context) avcodec_free_context(&context_.codec_context);
    if (context_.codec_context) avcodec_close(context_.codec_context);
    if (context_.format_context) avformat_free_context(context_.format_context);

    is_setup_ = false;
    is_exporting_ = false;
  }

  void bind() { fbo_.bind(); }
  void unbind() {
    fbo_.unbind();

    if (is_setup_ && is_exporting_ &&
        pbo_.readTo(&pixel_data_, fbo_.getID(), 0)) {
      write(pixel_data_.data());
    }
  }

  void write(const unsigned char *data) {
    auto ret = av_frame_make_writable(context_.frame);
    if (ret < 0) {
      std::cout << "frame is not writable" << std::endl;
      return;
    }

    const int in_linesize[1] = {context_.codec_context->width * 4};
    sws_scale(context_.sws_context, &data, in_linesize, 0,
              context_.codec_context->height, context_.frame->data,
              context_.frame->linesize);
    context_.frame->pts = context_.frame_index++;

    ret = avcodec_send_frame(context_.codec_context, context_.frame);
    if (ret < 0) {
      log::error("VideoExporter")
          << "Error sending a frame for encoding" << log::end();
      return;
    }

    flush();
  }

  void start() { is_exporting_ = true; }
  void stop() { is_exporting_ = false; }
  bool isExporting() const { return is_exporting_; }
  size_t getWidth() const { return fbo_.getWidth(); }
  size_t getHeight() const { return fbo_.getHeight(); }
  const gl::Texture2D &getTexture() const { return fbo_.getTexture(0); }

 private:
  bool flush() {
    int ret;
    do {
      AVPacket packet = {0};

      ret = avcodec_receive_packet(context_.codec_context, &packet);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;

      if (ret < 0) {
        log::error("VideoExporter") << "Error encoding a frame" << log::end();
        return false;
      }

      av_packet_rescale_ts(&packet, context_.codec_context->time_base,
                           context_.stream->time_base);
      packet.stream_index = context_.stream->index;

      ret = av_interleaved_write_frame(context_.format_context, &packet);
      av_packet_unref(&packet);
      if (ret < 0) {
        log::error("VideoExporter")
            << "Error while writing output packet" << log::end();
        return false;
      }
    } while (ret >= 0);

    return true;
  }
};
}  // namespace limas