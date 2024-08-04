#pragma once

#include "RSSyphonObject.h"
#include "gl/Texture2D.h"

namespace limas {
class RSSyphonClient {
 public:
  RSSyphonClient(/* args */);

  // void setup(const std::string &server_name, const std::string &app_name);
  void setApplicationName(const std::string &app_name);
  void setServerName(const std::string &server_name);

  void update();

  gl::Texture2D &getTexture() { return texture_; }

 protected:
  RSSyphonObject client_;
  RSSyphonObject latest_image_;

  gl::Texture2D texture_;
};

}  // namespace limas