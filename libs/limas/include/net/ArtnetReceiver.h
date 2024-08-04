#pragma once
#include "net/UdpServer.h"
#include "system/Exception.h"
#include "system/Thread.h"

namespace rs {
namespace net {

class ArtnetReceiver : public Thread {
 public:
  ArtnetReceiver() { dmx_.fill(0); }

  void start(boost::asio::io_service& io_service, int port = 6454) {
    io_service_ = &io_service;
    server_ = net::UdpServer::create(io_service, port);

    server_->setCallback([&](const string& packet) {
      static const int HEADER_LENGTH = 18;
      static const short OP_OUTPUT = 0x5000;

      if (packet.size() == HEADER_LENGTH + 512) {
        const char* data = packet.c_str();
        std::string protcol_id((char*)data, 7);
        if (protcol_id == "Art-Net") {
          if (data[8] == (OP_OUTPUT & 0xff) &&
              data[9] == ((OP_OUTPUT >> 8) & 0xff)) {
            const uint8_t* dmx_data =
                reinterpret_cast<const uint8_t*>(data + HEADER_LENGTH);

            auto locker = getLock();
            std::memcpy(dmx_.data(), dmx_data, dmx_.size() * sizeof(uint8_t));
          } else {
            log::error("ArtnetReceiver") << "Invalid OpCode" << log::end();
          }
        } else {
          log::error("ArtnetReceiver") << "Invalid Artnet header" << log::end();
        }
      }
    });
    startThread();
  }

  uint8_t getValue(size_t ch) const {
    auto locker = getLock();
    if ((ch - 1) < 0 || (ch - 1) > dmx_.size()) {
      throw rs::Exception("Invalid channel specified");
    }
    return dmx_[ch - 1];
  }

  std::vector<uint8_t> getValues(size_t ch, size_t size) const {
    auto locker = getLock();
    if ((ch - 1) < 0 || (ch + size - 1) > dmx_.size()) {
      throw rs::Exception("Invalid channel range specified.");
    }
    return std::vector<uint8_t>(dmx_.begin() + ch - 1,
                                dmx_.begin() + ch - 1 + size);
  }

 protected:
  boost::asio::io_service* io_service_;
  net::UdpServer::Ptr server_;
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