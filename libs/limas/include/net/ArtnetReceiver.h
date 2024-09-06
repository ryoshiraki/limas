#pragma once
#include "net/UdpServer.h"
#include "system/Exception.h"
#include "system/Thread.h"

namespace limas {
namespace net {

class ArtnetReceiver : public Thread {
 public:
  ArtnetReceiver() {}

  void reset() {
    auto locker = getLock();
    for (auto& universes : dmx_) {
      universes.second.fill(0);
    }
  }

  void reset(uint16_t universe) {
    auto locker = getLock();

    auto it = dmx_.find(universe);
    if (it == dmx_.end()) {
      return;
    }

    return it->second.fill(0);
  }

  void start(boost::asio::io_service& io_service,
             const std::vector<uint16_t>& universes, uint16_t port = 6454) {
    io_service_ = &io_service;
    server_ = net::UdpServer::create(io_service, port);
    universes_ = universes;
    port_ = port;

    for (auto universe : universes) {
      dmx_[universe].fill(0);
    }

    server_->setCallback([&](const string& packet) {
      static const int HEADER_LENGTH = 18;
      static const short OP_OUTPUT = 0x5000;

      if (packet.size() == HEADER_LENGTH + 512) {
        const char* data = packet.c_str();
        std::string protocol_id((char*)data, 7);
        if (protocol_id == "Art-Net") {
          if (data[8] == (OP_OUTPUT & 0xff) &&
              data[9] == ((OP_OUTPUT >> 8) & 0xff)) {
            uint16_t universe = data[14] | (data[15] << 8);

            auto it = dmx_.find(universe);
            if (it == dmx_.end()) {
              return;
            }

            const uint8_t* dmx_data =
                reinterpret_cast<const uint8_t*>(data + HEADER_LENGTH);

            auto locker = getLock();
            std::memcpy(it->second.data(), dmx_data,
                        it->second.size() * sizeof(uint8_t));
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

  uint8_t getValue(uint16_t universe, uint16_t ch) const {
    auto locker = getLock();

    auto it = dmx_.find(universe);

    if (it == dmx_.end()) {
      throw limas::Exception("Invalid universe specified");
      return 0;
    }

    if ((ch - 1) < 0 || (ch - 1) > 512) {
      throw limas::Exception("Invalid channel specified");
    }

    return it->second[ch - 1];
  }

  std::vector<uint8_t> getValues(uint16_t universe, uint16_t ch,
                                 uint16_t size) const {
    auto locker = getLock();

    auto it = dmx_.find(universe);

    if (it == dmx_.end()) {
      throw limas::Exception("Invalid universe specified");
      return {};
    };

    if ((ch - 1) < 0 || (ch + size - 1) > 512) {
      throw limas::Exception("Invalid channel range specified.");
    }

    return std::vector<uint8_t>(it->second.begin() + ch - 1,
                                it->second.begin() + ch - 1 + size);
  }

  const std::array<uint8_t, 512>& getValues(uint16_t universe) const {
    auto locker = getLock();

    auto it = dmx_.find(universe);

    if (it == dmx_.end()) {
      throw limas::Exception("Invalid universe specified");
      return {};
    };

    return it->second;
  }

  const std::vector<uint16_t>& getUniverses() const { return universes_; }
  uint16_t getPort() const { return port_; }

 protected:
  boost::asio::io_service* io_service_;
  net::UdpServer::Ptr server_;
  std::map<uint16_t, std::array<uint8_t, 512>> dmx_;
  std::vector<uint16_t> universes_;
  uint16_t port_;

 private:
  void threadedFunction() {
    io_service_->run();
    while (isThreadRunning()) {
    }
  }
};

}  // namespace net
}  // namespace limas
