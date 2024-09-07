#pragma once
#include "net/UdpClient.h"
#include "system/Exception.h"
#include "system/Thread.h"

// INCOMPLETED

namespace limas {
namespace net {
class ArtnetSender {
 public:
  void setup(const std::string& ip, uint16_t universe, int port = 6454) {
    client_ = std::make_shared<UdpClient>(ip, port);
    ip_ = ip;
    port_ = port;
    universe_ = universe;
    reset();
  }

  void reset() { dmx_.fill(0); }

  void set(uint16_t ch, uint8_t val) {
    if ((ch - 1) < 0 || (ch - 1) >= 512) {
      throw limas::Exception("Invalid channel range specified.");
    }
    dmx_[ch - 1] = val;
  }

  void set(uint16_t ch, std::vector<uint8_t> values) {
    if ((ch - 1) < 0 || (ch + values.size() - 1) > 512) {
      throw limas::Exception("Invalid channel range specified.");
    }
    std::copy(values.begin(), values.end(), dmx_.begin() + ch - 1);
  }

  uint8_t getValue(uint16_t ch) const {
    if ((ch - 1) < 0 || (ch - 1) >= 512) {
      throw limas::Exception("Invalid channel range specified.");
    }
    return dmx_[ch - 1];
  }

  std::vector<uint8_t> getValues(uint16_t ch, size_t size) const {
    if ((ch - 1) < 0 || (ch + size - 1) > 512) {
      throw limas::Exception("Invalid channel range specified.");
    }
    std::vector<uint8_t> values(size);
    std::copy(dmx_.begin() + ch - 1, dmx_.begin() + ch - 1 + size,
              values.begin());
    return values;
  }

  const std::string& getIp() const { return ip_; }
  u_int16_t getPort() const { return port_; }
  u_int16_t getUniverse() const { return universe_; }

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
    packet[12] = 0x00;                     // Sequence
    packet[13] = 0x00;                     // Physical
    packet[14] = universe_ & 0xFF;         // Universe (low byte)
    packet[15] = (universe_ >> 8) & 0xFF;  // Universe (high byte)
    packet[16] = 0x02;                     // Data length high byte (512)
    packet[17] = 0x00;                     // Data length low byte

    std::copy(dmx_.begin(), dmx_.end(), packet.begin() + 18);
    client_->send(std::vector<unsigned char>(packet.begin(), packet.end()));
  }

 protected:
  std::shared_ptr<net::UdpClient> client_;
  std::string ip_;
  uint16_t universe_ = 0;
  uint16_t port_ = 6454;
  std::array<uint8_t, 512> dmx_;
};

}  // namespace net
}  // namespace limas