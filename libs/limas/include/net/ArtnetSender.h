#pragma once
#include "net/UdpClient.h"
#include "system/Exception.h"
#include "system/Thread.h"

// INCOMPLETED

namespace rs {
namespace net {
class ArtnetSender {
 public:
  ~ArtnetSender() { stop(); }

  void start(boost::asio::io_service& io_service, const std::string& ip,
             int port = 6454) {}

  void set(uint16_t ch, uint8_t val) {
    auto locker = getLock();
    if ((ch - 1) < 0 || (ch - 1) > dmx_.size()) {
      throw rs::Exception("Invalid channel range specified.");
    }
    dmx_[ch - 1] = val;
  }

 protected:
  boost::asio::io_service* io_service_;
  net::UdpClient::Ptr client_;
  std::array<uint8_t, 512> dmx_;

 private:
  void threadedFunction() {
    io_service_->run();
    while (isThreadRunning()) {
    }
  }
};

}  // namespace net
}  // namespace rs