#pragma once
#include "ip/UdpSocket.h"
#include "osc/OscOutboundPacketStream.h"

namespace rs {
namespace net {

class OscSender {
 public:
  virtual ~OscSender() { delete socket_; }

  void setup(const std::string& ip, int port) {
    socket_ = new UdpTransmitSocket(IpEndpointName(ip.c_str(), port));
  }

  template <typename... Args>
  void send(const std::string& address, const Args&... args) {
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream packet(buffer, OUTPUT_BUFFER_SIZE);
    packet << osc::BeginMessage(address.c_str());
    appendArgs(packet, args...);
    packet << osc::EndMessage;
    socket_->Send(packet.Data(), packet.Size());
  }

 private:
  static constexpr int OUTPUT_BUFFER_SIZE = 4096;  // 1024
  UdpTransmitSocket* socket_;

  void appendArgs(osc::OutboundPacketStream&) {}

  template <typename T, typename... Rest>
  void appendArgs(osc::OutboundPacketStream& packet, const T& first,
                  const Rest&... rest) {
    packet << first;
    appendArgs(packet, rest...);
  }

  template <typename T, typename... Rest>
  void appendArgs(osc::OutboundPacketStream& packet,
                  const std::vector<T>& first, const Rest&... rest) {
    for (auto& e : first) packet << e;
    appendArgs(packet, rest...);
  }

  template <typename T, size_t N, typename... Rest>
  void appendArgs(osc::OutboundPacketStream& packet,
                  const std::array<T, N>& first, const Rest&... rest) {
    for (const auto& e : first) packet << e;
    appendArgs(packet, rest...);
  }
};

// Parameter<int> param;
// OscClient client;
// client.setup("localhost", 54321);
// param.onChange([=](const int& a) {
//   client.send("/test", a);
// });

}  // namespace net
}  // namespace rs
