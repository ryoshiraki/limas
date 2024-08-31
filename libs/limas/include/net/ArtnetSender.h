#pragma once
#include "net/UdpClient.h"
#include "system/Exception.h"
#include "system/Thread.h"

// INCOMPLETED

namespace limas {
namespace net {
class ArtnetSender {
 public:
  void setup(boost::asio::io_service& io_service, const std::string& ip,
             int port = 6454) {
    client_ = std::make_shared<UdpClient>(io_service, ip, port);
    reset();
  }

  void reset() { dmx_.fill(0); }

  void set(uint16_t ch, uint8_t val) {
    if ((ch - 1) < 0 || (ch - 1) > dmx_.size()) {
      throw limas::Exception("Invalid channel range specified.");
    }
    dmx_[ch - 1] = val;
  }

  uint8_t getValue(uint16_t ch) const {
    if ((ch - 1) < 0 || (ch - 1) > dmx_.size()) {
      throw limas::Exception("Invalid channel range specified.");
    }
    return dmx_[ch - 1];
  }

  void send() const {
    std::array<uint8_t, 530> packet;  // Art-Net header + DMX data (512 bytes)
    std::fill(packet.begin(), packet.end(), 0);

    // Art-Net Header
    std::string header = "Art-Net";
    std::copy(header.begin(), header.end(), packet.begin());
    packet[8] = 0x00;  // OpCode (ArtDMX)
    packet[9] = 0x50;
    packet[10] = 0x0E;  // Protocol version
    packet[11] = 0x00;
    packet[12] = 0x00;                       // Sequence
    packet[13] = 0x00;                       // Physical
    packet[14] = 0x00;                       // Universe (low byte)
    packet[15] = 0x00;                       // Universe (high byte)
    packet[16] = (dmx_.size() >> 8) & 0xFF;  // Data length high byte
    packet[17] = dmx_.size() & 0xFF;         // Data length low byte

    std::copy(dmx_.begin(), dmx_.end(), packet.begin() + 18);
    client_->send(std::vector<unsigned char>(packet.begin(), packet.end()));
  }

 protected:
  boost::asio::io_service* io_service_;
  std::shared_ptr<net::UdpClient> client_;
  std::array<uint8_t, 512> dmx_;
};

}  // namespace net
}  // namespace limas