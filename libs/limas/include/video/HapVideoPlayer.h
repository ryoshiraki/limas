#pragma once

#include <dispatch/dispatch.h>

extern "C" {
#include "hap.h"
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
#include "gl/VboMesh.h"
#include "graphics/Pixels.h"
#include "primitives/Rectangle.h"
#include "system/Thread.h"
#include "utils/Stopwatch.h"

namespace limas {
class HapVideoPlayer : public Thread {
  static int roundUpToMultipleOf4(int n) {
    if (0 != (n & 3)) n = (n + 3) & ~3;
    return n;
  }

  static bool frameMatchesStream(unsigned int frame, uint32_t stream) {
    switch (stream) {
      case MKTAG('H', 'a', 'p', '1'):
        if (frame == HapTextureFormat_RGB_DXT1) return true;
        break;
      case MKTAG('H', 'a', 'p', '5'):
        if (frame == HapTextureFormat_RGBA_DXT5) return true;
        break;
      case MKTAG('H', 'a', 'p', 'Y'):
        if (frame == HapTextureFormat_YCoCg_DXT5) return true;
      default:
        break;
    }
    return false;
  }

  static void doDecode(HapDecodeWorkFunction function, void *p, unsigned int count, void *info) {
    dispatch_apply(count, dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^(size_t i) {
      function(p, i);
    });
  }

  struct Context {
    int width;
    int height;
    int buffer_size;  //

    AVFormatContext *format_context;
    AVCodecContext *codec_context;
    SwsContext *sws_context;

    int stream_index;
    double time_base;
    double frame_rate;
    double duration;
  };
  Context context_;

  struct DecodedFrame {
    std::vector<char> buffer;
    int64_t pts;
  };
  std::deque<DecodedFrame *> frame_queues_;
  DecodedFrame *last_frame_;

  struct VideoState {
    VideoState()
        : b_new_frame(false),
          b_loaded(false),
          b_playing(false),
          b_loop(true),
          speed(1),
          b_request_seek(false),
          seek_time(0),
          offset_time(0) {}

    bool b_new_frame;
    bool b_loaded;
    bool b_playing;
    bool b_loop;
    float speed;
    bool b_request_seek;
    double seek_time;    // seconds
    double offset_time;  // seconds
  };
  VideoState state_;

  gl::Fbo::Ptr fbo_;
  gl::Texture2D::Ptr tex_;
  gl::Shader::Ptr shader_;
  gl::VboMesh plane_;

  PreciseStopwatch stopwatch_;

 public:
  HapVideoPlayer() {
    plane_ = prim::Rectangle(-1, -1, 2, 2);
    shader_ = gl::Shader::create(fs::getCommonResourcesPath() + "shaders/thru.vert",
                                 fs::getCommonResourcesPath() + "shaders/hap.frag");
  }
  virtual ~HapVideoPlayer() { close(); }

  void close() {
    stopThread();

    if (context_.sws_context) sws_freeContext(context_.sws_context);
    if (context_.codec_context) avcodec_free_context(&context_.codec_context);
    if (context_.codec_context) avcodec_close(context_.codec_context);
    if (context_.format_context) avformat_free_context(context_.format_context);

    state_ = VideoState();
    frame_queues_.clear();
    stopwatch_.reset();
  }

  bool load(const std::string &filename, size_t thread_count = 8) {
    close();

    do {
      if (!(context_.format_context = avformat_alloc_context())) {
        log::error("VideoPlayer") << "Couldn't create AVFormatContext" << log::end();
        break;
      }

      if (avformat_open_input(&context_.format_context, filename.c_str(), nullptr, nullptr) != 0) {
        log::error("VideoPlayer") << "Couldn't open video file" << log::end();
        break;
      }

      if (avformat_find_stream_info(context_.format_context, nullptr) < 0) {
        log::error("VideoPlayer") << "Couldn't retrieve stream info" << log::end();
        break;
      }

      av_dump_format(context_.format_context, 0, filename.c_str(), 0);

      AVCodec *codec;
      AVCodecParameters *codec_param;
      context_.stream_index = -1;
      for (int i = 0; i < context_.format_context->nb_streams; i++) {
        codec_param = context_.format_context->streams[i]->codecpar;
        codec = const_cast<AVCodec *>(avcodec_find_decoder(codec_param->codec_id));
        if (!codec) continue;

        if (codec_param->codec_type == AVMEDIA_TYPE_VIDEO &&
            codec_param->codec_id == AV_CODEC_ID_HAP) {
          context_.stream_index = i;
          context_.width = roundUpToMultipleOf4(codec_param->width);
          context_.height = roundUpToMultipleOf4(codec_param->height);
          context_.time_base = av_q2d(context_.format_context->streams[i]->time_base);
          context_.frame_rate = av_q2d(context_.format_context->streams[i]->r_frame_rate);
          context_.duration = context_.format_context->duration / AV_TIME_BASE;
          break;
        }
      }

      if (context_.stream_index == -1) {
        log::error("VideoPlayer") << "Couldn't find video stream" << log::end();
        break;
      }

      if (!(context_.codec_context = avcodec_alloc_context3(codec))) {
        log::error("VideoPlayer") << "Couldn't create AVCodecContext" << log::end();
        break;
      }
      context_.codec_context->thread_count = thread_count;

      if (avcodec_parameters_to_context(context_.codec_context, codec_param) < 0) {
        log::error("VideoPlayer") << "Couldn't initialize AVCodecContext" << log::end();
        break;
      }

      if (avcodec_open2(context_.codec_context, codec, nullptr) < 0) {
        log::error("VideoPlayer") << "Couldn't open codec" << log::end();
        break;
      }

      context_.buffer_size = context_.width * context_.height;
      GLenum internal_format = GL_RGBA8;
      switch (context_.codec_context->codec_tag) {
        case MKTAG('H', 'a', 'p', '1'):
          internal_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
          context_.buffer_size = context_.width * context_.height * 0.5;
          break;

        case MKTAG('H', 'a', 'p', '5'):
        case MKTAG('H', 'a', 'p', 'Y'):
          internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
          break;

        case MKTAG('H', 'a', 'p', 'M'):
          // TODO: HapM
          break;

        default:
          break;
      }

      tex_ = gl::Texture2D::create(context_.width, context_.height, internal_format, GL_BGRA,
                                   GL_UNSIGNED_INT_8_8_8_8_REV);

#if 1
      tex_->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);
      tex_->unbind();
#endif

      fbo_ = gl::Fbo::create(context_.width, context_.height);
      fbo_->attachColor(GL_RGBA8);
      fbo_->bind();
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT);
      fbo_->unbind();

      shader_->bind();
      shader_->setUniform1i("u_YCoCg", internal_format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
      shader_->unbind();

      // int num_buffers = context_.frame_rate / 10.0;
      // frame_buffers_.resize(num_buffers);
      // for (int i = 0; i < frame_buffers_.size(); i++) {
      //   DecodedFrame *frame = new DecodedFrame();
      //   frame->buffer.resize(buffer_size);
      //   frame_buffers_[i] = frame;
      // }
      state_.b_loaded = true;

      startThread();

      return true;
    } while (false);

    close();

    return false;
  }

  void update() {
    if (!state_.b_loaded) return;

    double current_time = getTime();
    if (state_.b_loop && current_time > context_.duration) {
      seekTime(0);
      play();
      return;
    }

    DecodedFrame *frame;

    {
      auto locker = getLock();
      while (frame_queues_.size()) {
        frame = frame_queues_.front();
        if (current_time > (frame->pts * context_.time_base)) {
          frame_queues_.pop_front();
          notify();
        } else {
          break;
        }
      }
      if (frame_queues_.empty()) return;
    }

    if (frame != last_frame_) {
      tex_->bind();
      glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
      glTextureRangeAPPLE(GL_TEXTURE_2D, frame->buffer.size(), frame->buffer.data());
      glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_->getWidth(), tex_->getHeight(),
                                tex_->getInternalFormat(),
                                static_cast<GLsizei>(frame->buffer.size()), frame->buffer.data());
      glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_FALSE);
      tex_->unbind();

      fbo_->bind();
      shader_->bind();
      shader_->setUniformTexture("u_tex", *tex_, 0);
      plane_.draw(GL_TRIANGLE_FAN);
      shader_->unbind();
      fbo_->unbind();

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

  const gl::Texture2D::Ptr &getTexture() const { return fbo_->getTexture(0); }
  gl::Texture2D::Ptr &getTexture() { return fbo_->getTexture(0); }

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

  double getTime() { return stopwatch_.getElapsedInSeconds() * state_.speed + state_.offset_time; }
  double getPosition() { return getTime() / getDuration(); }

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
      int ret = av_seek_frame(context_.format_context, context_.stream_index, pts, AVSEEK_FLAG_ANY);
      if (ret < 0) {
        log::error("av_seek_frame") << av_err2str(ret) << log::end();
        return;
      }

      avcodec_flush_buffers(context_.codec_context);
    }
  }

  void threadedFunction() {
    AVPacket *packet = av_packet_alloc();
    if (!packet) {
      log::error("VideoPlayer") << "Couldn't allocate packet" << log::end();
      return;
    }

    int num_buffers = context_.frame_rate / 10.0;
    std::vector<DecodedFrame> frame_buffers_(num_buffers);
    for (int i = 0; i < frame_buffers_.size(); i++) {
      frame_buffers_[i].buffer.resize(context_.buffer_size);
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
        unsigned int tex_count;
        unsigned int hap_result = HapGetFrameTextureCount(packet->data, packet->size, &tex_count);
        if (hap_result == HapResult_No_Error && tex_count == 1) {  // TODO: Hap Q+A
          unsigned int tex_format;
          hap_result = HapGetFrameTextureFormat(packet->data, packet->size, 0, &tex_format);
          if (hap_result == HapResult_No_Error &&
              !frameMatchesStream(tex_format, context_.codec_context->codec_tag)) {
            hap_result = HapResult_Bad_Frame;
          }

          if (hap_result == HapResult_No_Error) {
            auto &frame = frame_buffers_[current_frame_index_];

            unsigned long bytes_used;
            hap_result = HapDecode(
                packet->data, packet->size, 0, doDecode, NULL, frame.buffer.data(),
                static_cast<unsigned long>(frame.buffer.size()), &bytes_used, &tex_format);
            frame.pts = packet->pts;

            {
              auto locker = getLock();
              waitFor(locker, [&] { return frame_queues_.size() <= frame_buffers_.size(); });
              frame_queues_.push_back(&frame);
              current_frame_index_++;
              current_frame_index_ %= frame_buffers_.size();
            }
          } else {
            log::warn("VideoPlayer") << "Invalid frame" << log::end();
          }
        }
      }

      av_packet_unref(packet);
    }

    av_packet_free(&packet);
  }
};

}  // namespace limas