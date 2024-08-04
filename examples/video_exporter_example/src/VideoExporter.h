#pragma once

#include <array>
#include <condition_variable>
#include <queue>
#include <thread>

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

namespace rs {
class VideoExporter {
  gl::Fbo::Ptr fbo_;
  gl::PboPacker::Ptr pbo_;

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

 public:
  ~VideoExporter() { close(); }

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
  }

  bool setup(size_t width, size_t height, int fps,
             const std::string &filepath) {
    do {
      fbo_ = gl::Fbo::create(width, height);
      fbo_->attachColor(GL_RGB8);
      pbo_ = gl::PboPacker::create(fbo_->getWidth(), fbo_->getHeight(),
                                   fbo_->getTextures()[0]->getInternalFormat());

      avformat_alloc_output_context2(&context_.format_context, nullptr, nullptr,
                                     filepath.c_str());
      if (!context_.format_context) {
        log::error("VideoExporter")
            << "Couldn't allocate format context" << log::end();
        break;
      }

      const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
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
      context_.codec_context->codec_id =
          context_.format_context->oformat->video_codec;
      context_.codec_context->bit_rate = 96000;
      context_.codec_context->width = width;
      context_.codec_context->height = height;
      context_.codec_context->time_base = context_.stream->time_base;
      context_.codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
      context_.codec_context->gop_size = 12;
      context_.codec_context->max_b_frames = 2;

      // generate global header when the format requires it
      if (context_.format_context->oformat->flags & AVFMT_GLOBALHEADER) {
        context_.format_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
      }

      AVDictionary *codec_options = nullptr;
      av_dict_set(&codec_options, "preset", "medium", 0);
      av_dict_set(&codec_options, "crf", "22", 0);
      av_dict_set(&codec_options, "profile", "high", 0);
      av_dict_set(&codec_options, "level", "4.0", 0);

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

      if (av_frame_get_buffer(context_.frame, 32) < 0) {  // what is 32?
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
                AV_PIX_FMT_RGB24,  // src
                context_.codec_context->width, context_.codec_context->height,
                AV_PIX_FMT_YUV420P,  // dst
                SWS_BICUBIC, nullptr, nullptr, nullptr))) {
        log::error("VideoExporter") << "Couldn't get sws context" << log::end();
        break;
      }

      av_dump_format(context_.format_context, 0, filepath.c_str(), 1);

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

      context_.frame_index = 0;
      is_setup_ = true;
      return true;
    } while (false);

    close();
    return false;
  }

  void bind() { fbo_->bind(); }
  void unbind() {
    fbo_->unbind();

    Pixels2D pixels;
    if (pbo_->readToPixels(&pixels, fbo_->getID(), 0)) {
      log::info("frame") << context_.frame_index << log::end();

      auto ret = av_frame_make_writable(context_.frame);
      if (ret < 0) {
        std::cout << "frame is not writable" << std::endl;
        return;
      }

      const int in_linesize[1] = {context_.codec_context->width * 3};

      const unsigned char *data = &pixels.getData()[0];
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
  }

  const gl::Texture2D::Ptr &getTexture() const { return fbo_->getTexture(0); }

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
}  // namespace rs