#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixdesc.h"
#include "libswscale/swscale.h"
}

#include "gl/Fbo.h"
#include "gl/Shader.h"
#include "gl/Texture2D.h"
#include "graphics/Pixels.h"
#include "system/Thread.h"
#include "utils/Stopwatch.h"

namespace limas {
class VideoPlayer : public Thread {
  struct Context {
    int width;
    int height;

    AVFormatContext *format_context = nullptr;
    AVCodecContext *codec_context = nullptr;
    SwsContext *sws_context = nullptr;

    int stream_index = -1;
    double time_base = 0.0;
    double frame_rate = 0.0;
    double duration = 0.0;
  } context_;

  std::deque<AVFrame *> frame_queues_;
  AVFrame *last_frame_ = nullptr;
  struct VideoState {
    bool b_new_frame = false;
    bool b_loaded = false;
    bool b_playing = false;
    bool b_loop = true;
    float speed = 1.0f;
    bool b_request_seek = false;
    double seek_time = 0.0;
    double offset_time = 0.0;
  } state_;

  gl::Texture2D tex_;
  Pixels2D pixels_;
  PreciseStopwatch stopwatch_;

 public:
  VideoPlayer() {}
  virtual ~VideoPlayer() { close(); }

  void close() {
    stopThread();

    if (context_.sws_context) {
      sws_freeContext(context_.sws_context);
      context_.sws_context = nullptr;
    }
    if (context_.codec_context) {
      avcodec_free_context(&context_.codec_context);
      context_.codec_context = nullptr;
    }
    if (context_.format_context) {
      avformat_close_input(&context_.format_context);
      context_.format_context = nullptr;
    }

    state_ = VideoState();
    frame_queues_.clear();
    stopwatch_.reset();
  }

  bool load(const std::string &filename, size_t thread_count = 8) {
    close();

    if (!(context_.format_context = avformat_alloc_context())) {
      logger::error("VideoPlayer")
          << "Couldn't create AVFormatContext" << logger::end();
      return false;
    }

    if (avformat_open_input(&context_.format_context, filename.c_str(), nullptr,
                            nullptr) != 0) {
      logger::error("VideoPlayer")
          << "Couldn't open " << filename << logger::end();
      return false;
    }

    if (avformat_find_stream_info(context_.format_context, nullptr) < 0) {
      logger::error("VideoPlayer")
          << "Couldn't retrieve stream info" << logger::end();
      return false;
    }

    av_dump_format(context_.format_context, 0, filename.c_str(), 0);

    const AVCodec *codec = nullptr;
    AVCodecParameters *codec_param = nullptr;

    for (unsigned int i = 0; i < context_.format_context->nb_streams; i++) {
      codec_param = context_.format_context->streams[i]->codecpar;
      codec = avcodec_find_decoder(codec_param->codec_id);

      if (codec && codec_param->codec_type == AVMEDIA_TYPE_VIDEO) {
        // H.264の場合、VideoToolboxを優先
        if (std::string(codec->name) == "h264" &&
            avcodec_find_decoder_by_name("h264_videotoolbox")) {
          codec = avcodec_find_decoder_by_name("h264_videotoolbox");
        }

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
      logger::error("VideoPlayer")
          << "Couldn't find video stream" << logger::end();
      return false;
    }

    if (!(context_.codec_context = avcodec_alloc_context3(codec))) {
      logger::error("VideoPlayer")
          << "Couldn't create AVCodecContext" << logger::end();
      return false;
    }

    context_.codec_context->thread_count = thread_count;

    if (avcodec_parameters_to_context(context_.codec_context, codec_param) <
        0) {
      logger::error("VideoPlayer")
          << "Couldn't initialize AVCodecContext" << logger::end();
      return false;
    }

    if (avcodec_open2(context_.codec_context, codec, nullptr) < 0) {
      logger::error("VideoPlayer") << "Couldn't open codec" << logger::end();
      return false;
    }

    if (!(context_.sws_context = sws_getContext(
              context_.width, context_.height, context_.codec_context->pix_fmt,
              context_.width, context_.height, AV_PIX_FMT_RGB24,
              SWS_FAST_BILINEAR,  // SWS_BICUBIC,
              nullptr, nullptr, nullptr))) {
      logger::error("VideoPlayer")
          << "Couldn't get sws context" << logger::end();
      return false;
    }

    tex_.allocate(context_.width, context_.height, GL_RGB8);
    tex_.setMinFilter(GL_LINEAR);
    tex_.setMagFilter(GL_LINEAR);

    pixels_.allocate(context_.width, context_.height, 3);

    state_.b_loaded = true;

    startThread([this]() { this->threadedFunction(); });

    return true;
  }

  void update() {
    if (!state_.b_loaded) return;

    double current_time = getTime();
    if (state_.b_loop && current_time > context_.duration) {
      seekTime(0);
      play();
      return;
    }

    AVFrame *frame;

    {
      bool b_should_notify = false;
      auto locker = getLock();
      while (frame_queues_.size()) {
        frame = frame_queues_.front();
        if (current_time > (frame->pts * context_.time_base)) {
          frame_queues_.pop_front();
          b_should_notify = true;
        } else {
          break;
        }
      }
      if (b_should_notify) notify();
      if (frame_queues_.empty()) return;
    }

    if (frame != last_frame_) {
      int size[4] = {
          static_cast<int>(pixels_.getNumChannels() * pixels_.getWidth()), 0, 0,
          0};
      uint8_t *data[4] = {&pixels_.getData()[0], 0, 0, 0};

      if (sws_scale(context_.sws_context, frame->data, frame->linesize, 0,
                    pixels_.getHeight(), data, size) > 0) {
        tex_.loadData(&pixels_.getData()[0]);
      }
      last_frame_ = frame;
      state_.b_new_frame = true;
    } else {
      state_.b_new_frame = false;
    }
  }

  void play() {
    stopwatch_.start();
    {
      auto locker = getLock();
      state_.b_playing = true;
      notify();
    }
  }

  void pause() {
    stopwatch_.stop();
    {
      auto locker = getLock();
      state_.b_playing = false;
    }
  }

  void setLoop(bool loop) { state_.b_loop = loop; }
  bool isLoop() const { return state_.b_loop; }

  void setSpeed(float speed) {
    state_.offset_time = getTime();
    state_.speed = speed;
    stopwatch_.restart();
  }
  float getSpeed() const { return state_.speed; }

  void seekTime(double seconds) {
    stopwatch_.restart();
    {
      auto locker = getLock();
      state_.b_request_seek = true;
      state_.seek_time = seconds;
      state_.offset_time = seconds;
      frame_queues_.clear();
      notify();
    }
  }

  void seekFrame(int64_t frame) {
    double seconds = frame * getFrameRate();
    seekTime(seconds);
  }

  void seekPosition(double t) {
    double seconds = getDuration() * t;
    seekTime(seconds);
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

  bool isPlaying() {
    auto locker = getLock();
    return state_.b_playing;
  }

  double getTime() {
    return stopwatch_.getElapsedInSeconds() * state_.speed + state_.offset_time;
  }

  double getPosition() { return getTime() / getDuration(); }

 protected:
 private:
  void waitForPlaying() {
    auto locker = getLock();
    waitFor(locker, [this] { return state_.b_playing; });
  }

  void waitForSeek() {
    auto locker = getLock();
    if (state_.b_request_seek) {
      state_.b_request_seek = false;

      int64_t pts = state_.seek_time / context_.time_base;
      int ret = av_seek_frame(context_.format_context, context_.stream_index,
                              pts, AVSEEK_FLAG_ANY);
      if (ret < 0) {
        logger::error("av_seek_frame") << av_err2str(ret) << logger::end();
        return;
      }

      avcodec_flush_buffers(context_.codec_context);
    }
  }

  void threadedFunction() {
    AVPacket *packet = av_packet_alloc();
    if (!packet) {
      logger::error("VideoPlayer")
          << "Couldn't allocate packet" << logger::end();
      return;
    }

    std::vector<AVFrame *> frame_buffers;
    int num_buffers = context_.frame_rate / 10.0;
    for (int i = 0; i < num_buffers; i++) {
      AVFrame *frame = av_frame_alloc();
      if (!av_image_alloc(frame->data, frame->linesize, context_.width,
                          context_.height, context_.codec_context->pix_fmt,
                          1)) {
        logger::error("VideoPlayer")
            << "Couldn't allocate frame" << logger::end();
        continue;
      }
      frame_buffers.push_back(frame);
    }

    int current_frame_index_ = 0;

    while (isThreadRunning()) {
      waitForPlaying();
      waitForSeek();

      int ret = av_read_frame(context_.format_context, packet);
      if (ret < 0) {
        auto locker = getLock();
        if (frame_queues_.empty()) state_.b_playing = false;
        continue;
      }

      if (packet->stream_index == context_.stream_index) {
        ret = avcodec_send_packet(context_.codec_context, packet);
        if (ret < 0) {
          logger::error("avcodec_send_packet")
              << av_err2str(ret) << logger::end();
          continue;
        }

        while (ret >= 0) {
          AVFrame *frame = frame_buffers[current_frame_index_];

          av_frame_unref(frame);

          ret = avcodec_receive_frame(context_.codec_context, frame);
          if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
          } else if (ret < 0) {
            logger::error("avcodec_receive_frame")
                << av_err2str(ret) << logger::end();
          }

          {
            auto locker = getLock();
            waitFor(locker, [&] {
              return frame_queues_.size() <= frame_buffers.size();
            });
            frame_queues_.push_back(frame);
          }

          current_frame_index_++;
          current_frame_index_ %= frame_buffers.size();
        }
      }

      av_packet_unref(packet);
    }

    for (auto &f : frame_buffers) av_frame_free(&f);
    frame_buffers.clear();
    av_packet_free(&packet);
  }
};

}  // namespace limas
