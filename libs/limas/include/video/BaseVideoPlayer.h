#pragma once

#include "gl/Texture2D.h"
#include "system/Thread.h"
#include "utils/Stopwatch.h"

namespace rs {
class BaseVideoPlayer {
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

  gl::Texture2D::Ptr tex_;
  PreciseStopwatch stopwatch_;

 public:
  BaseVideoPlayer() {}
  virtual ~BaseVideoPlayer() { close(); }

  virtual void close() = 0;
  virtual bool load(const std::string &filename, size_t thread_count = 8) = 0;
  virtual void update() = 0;

  virtual void play() {
    stopwatch_.start();
    state_.b_playing = true;
  }

  virtual void pause() {
    stopwatch_.stop();
    state_.b_playing = false;
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
    state_.b_request_seek = true;
    state_.seek_time = seconds;
    state_.offset_time = seconds;
  }

  void seekFrame(int64_t frame) {
    double seconds = frame * getFrameRate();
    seekTime(seconds);
  }

  void seekPosition(double t) {
    double seconds = getDuration() * t;
    seekTime(seconds);
  }

  bool isLoaded() const { return state_.b_loaded; }
  bool isPlaying() { return state_.b_playing; }
  bool isFrameNew() const { return state_.b_new_frame; }
  gl::Texture2D::Ptr &getTexture() { return tex_; }
  size_t getWidth() const { return context_.width; }
  size_t getHeight() const { return context_.height; }
  double getDuration() const { return context_.duration; }
  double getFrameRate() const { return context_.frame_rate; }
  uint64_t getNumFrames() const { return getDuration() * getFrameRate(); }
  double getTime() {
    return stopwatch_.getElapsedInSeconds() * getSpeed() + state_.offset_time;
  }
  double getPosition() { return getTime() / getDuration(); }

 protected:
  virtual void threadedFunction() = 0;
};

}  // namespace rs
