#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixdesc.h"
#include "libswscale/swscale.h"
}

#include "gl/Fbo.h"
#include "gl/Shader.h"
#include "gl/Texture2D.h"
#include "graphics/Pixels.h"
#include "system/Thread.h"
#include "system/ThreadPool.h"
#include "utils/Stopwatch.h"

namespace limas {
class VideoSeeker {
  struct Context {
    int width;
    int height;

    AVFormatContext *format_context;
    AVCodecContext *codec_context;
    SwsContext *sws_context;

    int stream_index;
    double time_base;
    double frame_rate;
    double duration;
  };
  Context context_;

  AVFrame *frame_;
  AVPacket *packet_;
  AVFrame *last_frame_;

  struct VideoState {
    VideoState() : b_new_frame(false), b_loaded(false) {}

    bool b_new_frame;
    bool b_loaded;
  };
  VideoState state_;

  gl::Texture2D tex_;
  Pixels2D pixels_;
  PreciseStopwatch stopwatch_;

 public:
  VideoSeeker() {}
  virtual ~VideoSeeker() { close(); }

  void close() {
    if (context_.sws_context) sws_freeContext(context_.sws_context);
    if (context_.codec_context) avcodec_free_context(&context_.codec_context);
    if (context_.codec_context) avcodec_close(context_.codec_context);
    if (context_.format_context) avformat_free_context(context_.format_context);
    av_frame_free(&frame_);
    av_packet_free(&packet_);

    state_ = VideoState();
  }

  bool load(const std::string &filename, size_t thread_count = 8) {
    close();

    do {
      if (!(context_.format_context = avformat_alloc_context())) {
        log::error("VideoPlayer")
            << "Couldn't create AVFormatContext" << log::end();
        break;
      }

      if (avformat_open_input(&context_.format_context, filename.c_str(),
                              nullptr, nullptr) != 0) {
        log::error("VideoPlayer") << "Couldn't open " << filename << log::end();
        break;
      }

      if (avformat_find_stream_info(context_.format_context, nullptr) < 0) {
        log::error("VideoPlayer")
            << "Couldn't retrieve stream info" << log::end();
        break;
      }

      av_dump_format(context_.format_context, 0, filename.c_str(), 0);

      AVCodec *codec;
      AVCodecParameters *codec_param;
      context_.stream_index = -1;
      for (int i = 0; i < context_.format_context->nb_streams; i++) {
        codec_param = context_.format_context->streams[i]->codecpar;
        codec =
            const_cast<AVCodec *>(avcodec_find_decoder(codec_param->codec_id));
        if (!codec) continue;

        if (codec_param->codec_type == AVMEDIA_TYPE_VIDEO) {
          context_.stream_index = i;
          context_.width = codec_param->width;
          context_.height = codec_param->height;
          context_.time_base =
              av_q2d(context_.format_context->streams[i]->time_base);
          context_.frame_rate =
              av_q2d(context_.format_context->streams[i]->r_frame_rate);
          context_.duration = context_.format_context->duration / AV_TIME_BASE;
          break;
        }
      }

      if (context_.stream_index == -1) {
        log::error("VideoPlayer") << "Couldn't find video stream" << log::end();
        break;
      }

      if (!(context_.codec_context = avcodec_alloc_context3(codec))) {
        log::error("VideoPlayer")
            << "Couldn't create AVCodecContext" << log::end();
        break;
      }
      context_.codec_context->thread_count = thread_count;
      context_.codec_context->thread_type = FF_THREAD_FRAME | FF_THREAD_SLICE;

      if (avcodec_parameters_to_context(context_.codec_context, codec_param) <
          0) {
        log::error("VideoPlayer")
            << "Couldn't initialize AVCodecContext" << log::end();
        break;
      }

      if (avcodec_open2(context_.codec_context, codec, nullptr) < 0) {
        log::error("VideoPlayer") << "Couldn't open codec" << log::end();
        break;
      }

      AVPixelFormat format = context_.codec_context->pix_fmt;
      const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(format);
      if (desc->flags & AV_PIX_FMT_FLAG_RGB) {
        log::error("VideoPlayer")
            << av_get_pix_fmt_name(format) << " is not supported" << log::end();
        break;
      } else if (desc->nb_components == 1) {
        log::error("VideoPlayer")
            << av_get_pix_fmt_name(format) << " is not supported" << log::end();
        break;
      } else {
        if (!(context_.sws_context = sws_getContext(
                  context_.width, context_.height,
                  context_.codec_context->pix_fmt, context_.width,
                  //   context_.height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL,
                  //   NULL,
                  context_.height, AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL,
                  NULL,

                  NULL))) {
          log::error("VideoPlayer") << "Couldn't get sws context" << log::end();
          break;
        }

        tex_.allocate(context_.width, context_.height, GL_RGB8);
        tex_.setMinFilter(GL_LINEAR);
        tex_.setMagFilter(GL_LINEAR);
        std::vector<GLubyte> data(context_.width * context_.height * 3);
        std::fill(data.begin(), data.end(), 0);
        tex_.loadData(&data[0]);
      }

      pixels_.allocate(context_.width, context_.height, 3);

      state_.b_loaded = true;

      frame_ = av_frame_alloc();
      packet_ = av_packet_alloc();

      return true;
    } while (false);

    close();
    return false;
  }

  void update() {
    if (!state_.b_loaded) return;

    state_.b_new_frame = false;

    int max_attempts = 10;  // フレームを取得するための最大試行回数
    int attempts = 0;
    while (attempts < max_attempts) {
      int ret = av_read_frame(context_.format_context, packet_);
      if (ret < 0) {
        if (ret == AVERROR_EOF) {
          log::error("av_read_frame") << "EOF" << log::end();
        } else {
          log::error("av_read_frame") << av_err2str(ret) << log::end();
        }
        break;
      }

      if (packet_->stream_index == context_.stream_index) {
        ret = avcodec_send_packet(context_.codec_context, packet_);
        if (ret < 0) {
          log::error("avcodec_send_packet") << av_err2str(ret) << log::end();
          av_packet_unref(packet_);
          continue;
        }

        while ((ret = avcodec_receive_frame(context_.codec_context, frame_)) >=
               0) {
          processFrame();
          av_frame_unref(frame_);
          state_.b_new_frame = true;
          break;
        }

        if (ret != AVERROR(EAGAIN) && ret < 0) {
          log::error("avcodec_receive_frame") << av_err2str(ret) << log::end();
        }
      }
      av_packet_unref(packet_);
      if (state_.b_new_frame) break;

      attempts++;
    }
    // log::info("update") << "attempts: " << attempts << log::end();
  }

  void processFrame() {
    int size[4] = {
        static_cast<int>(pixels_.getNumChannels() * pixels_.getWidth()), 0, 0,
        0};
    uint8_t *data[4] = {&pixels_.getData()[0], nullptr, nullptr, nullptr};

    if (sws_scale(context_.sws_context, frame_->data, frame_->linesize, 0,
                  context_.height, data, size) > 0) {
      tex_.loadData(&pixels_.getData()[0]);
    }
  }

  void seekSeconds(double t) {
    int64_t pts = t / context_.time_base;
    int ret = av_seek_frame(context_.format_context, context_.stream_index, pts,
                            AVSEEK_FLAG_ANY);

    if (ret < 0) {
      log::error("av_seek_frame") << av_err2str(ret) << log::end();
    }

    avcodec_flush_buffers(context_.codec_context);
  }

  void seekFrame(int64_t frame) {
    double seconds = frame * getFrameRate();
    seekSeconds(seconds);
  }

  void seekPosition(double t) {
    double seconds = getDuration() * t;
    seekSeconds(seconds);
  }

  const Pixels2D &getPixels() const { return pixels_; }
  const gl::Texture2D &getTexture() const { return tex_; }
  gl::Texture2D &getTexture() { return tex_; }
  bool isFrameNew() const { return state_.b_new_frame; }
  size_t getWidth() const { return context_.width; }
  size_t getHeight() const { return context_.height; }
  double getDuration() const { return context_.duration; }
  double getFrameRate() const { return context_.frame_rate; }
  uint64_t getNumFrames() const { return getDuration() * getFrameRate(); }
  bool isLoaded() const { return state_.b_loaded; }
};

}  // namespace limas
