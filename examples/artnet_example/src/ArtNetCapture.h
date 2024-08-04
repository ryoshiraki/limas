#pragma once
#include "TrafficCapture.h"

namespace rs {
namespace net {

class ArtNetCapture : public TrafficCapture {
 protected:
  virtual void processPacket(const struct pcap_pkthdr* pkthdr,
                             const unsigned char* packet) override {
    const struct ip* ip_header = (struct ip*)(packet + 14);
    const struct udphdr* udp_header =
        (struct udphdr*)(packet + 14 + (ip_header->ip_hl << 2));
    const unsigned char* artnet_data =
        (unsigned char*)(packet + 14 + (ip_header->ip_hl << 2) +
                         sizeof(udphdr));

    if (ntohs(udp_header->uh_dport) == 6454 &&
        memcmp(artnet_data, "Art-Net", 7) == 0) {
      unsigned short opcode = artnet_data[8] | (artnet_data[9] << 8);
      if (opcode == 0x5000) {
        const uint8_t* dmx_data = artnet_data + 18;
        std::memcpy(dmx_.data(), dmx_data, dmx_.size() * sizeof(uint8_t));
      }
    }
  }

 public:
  using DmxData = std::array<uint8_t, 512>;

  void start(const char* device, int timeout) {
    TrafficCapture::start(device, "udp and port 6454", timeout);
  }

  DmxData getData() const {
    auto locker = getLock();
    return dmx_;
  }

  uint8_t getData(uint16_t ch) const {
    if (ch <= 0 || ch >= 513) {
      log::warn("ArtNetCapture") << "ch must be within 1~512" << log::end();
      return 0;
    }
    auto locker = getLock();
    return dmx_[ch - 1];
  }

 protected:
  DmxData dmx_;
};

}  // namespace net
}  // namespace rs