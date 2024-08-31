#pragma once
#include "ip/PacketListener.h"
#include "ip/UdpSocket.h"
#include "osc/OscOutboundPacketStream.h"

namespace limas {
namespace net {

// osc::OutboundPacketStream& operator<<(osc::OutboundPacketStream& p, int rhs)
// {
//   p << (osc::int32)rhs;
//   return p;
// }

class OscSender {
 public:
  virtual ~OscSender() = default;

  void setup(const std::string& ip, int port) {
    socket_ =
        std::make_unique<UdpTransmitSocket>(IpEndpointName(ip.c_str(), port));
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
  static constexpr int OUTPUT_BUFFER_SIZE = 4096;
  std::unique_ptr<UdpTransmitSocket> socket_;

  void appendArgs(osc::OutboundPacketStream&) {}

  void appendArgs(osc::OutboundPacketStream& packet, int first) {
    packet << (osc::int64)first;
  }

  void appendArgs(osc::OutboundPacketStream& packet, bool first) {
    packet << first;
  }

  void appendArgs(osc::OutboundPacketStream& packet, float first) {
    packet << first;
  }

  void appendArgs(osc::OutboundPacketStream& packet, double first) {
    packet << first;
  }

  void appendArgs(osc::OutboundPacketStream& packet, const std::string& first) {
    packet << first.c_str();
  }

  template <typename T, typename... Rest>
  void appendArgs(osc::OutboundPacketStream& packet, const T& first,
                  const Rest&... rest) {
    appendArgs(packet, first);    // 先頭の引数を処理
    appendArgs(packet, rest...);  // 残りの引数を再帰的に処理
  }

  template <typename T, typename... Rest>
  void appendArgs(osc::OutboundPacketStream& packet,
                  const std::vector<T>& first, const Rest&... rest) {
    for (const auto& e : first) {
      appendArgs(packet, e);  // vector内の要素を個別に処理
    }
    appendArgs(packet, rest...);  // 残りの引数を処理
  }

  template <typename T, size_t N, typename... Rest>
  void appendArgs(osc::OutboundPacketStream& packet,
                  const std::array<T, N>& first, const Rest&... rest) {
    for (const auto& e : first) {
      appendArgs(packet, e);  // array内の要素を個別に処理
    }
    appendArgs(packet, rest...);  // 残りの引数を処理
  }
};

// Parameter<int> param;
// OscClient client;
// client.setup("localhost", 54321);
// param.onChange([=](const int& a) {
//   client.send("/test", a);
// });

}  // namespace net
}  // namespace limas
