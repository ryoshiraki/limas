#pragma once
#include "net/UdpClient.h"
#include "oscpp/client.hpp"

namespace limas {
namespace net {

class OscSender {
 public:
  virtual ~OscSender() = default;

  void setup(const std::string& ip, uint16_t port) {
    client_ = std::make_unique<UdpClient>(ip, port);
    ip_ = ip;
    port_ = port;
  }

  template <typename... Args>
  void send(const std::string& address, const Args&... args) {
    std::vector<unsigned char> buffer(OUTPUT_BUFFER_SIZE);
    OSCPP::Client::Packet packet(buffer.data(), buffer.size());
    packet.openMessage(address.c_str(), sizeof...(Args));
    appendArgs(packet, args...);
    packet.closeMessage();
    client_->send(buffer);
  }

  const std::string& getIp() const { return ip_; }
  uint16_t getPort() const { return port_; }

 private:
  static constexpr int OUTPUT_BUFFER_SIZE = 4096;
  std::unique_ptr<net::UdpClient> client_;
  std::string ip_;
  uint16_t port_;

  void appendArgs(OSCPP::Client::Packet& packet) {}

  template <typename T>
  void appendArgs(OSCPP::Client::Packet& packet, T first);

  template <>
  void appendArgs<int>(OSCPP::Client::Packet& packet, int first) {
    packet.int32(first);
  }

  template <>
  void appendArgs<float>(OSCPP::Client::Packet& packet, float first) {
    packet.float32(first);
  }

  template <>
  void appendArgs<std::string>(OSCPP::Client::Packet& packet,
                               std::string first) {
    packet.string(first.c_str());
  }

  template <>
  void appendArgs<const char*>(OSCPP::Client::Packet& packet,
                               const char* first) {
    packet.string(first);
  }

  template <>
  void appendArgs<bool>(OSCPP::Client::Packet& packet, bool first) {
    packet.int32(static_cast<bool>(first));
  }

  template <>
  void appendArgs<unsigned int>(OSCPP::Client::Packet& packet,
                                unsigned int first) {
    packet.int32(first);
  }

  template <>
  void appendArgs<double>(OSCPP::Client::Packet& packet, double first) {
    packet.float32(first);
  }

  template <typename T, typename... Rest>
  void appendArgs(OSCPP::Client::Packet& packet, T first, const Rest&... rest) {
    appendArgs(packet, first);
    appendArgs(packet, rest...);
  }

  template <typename T, typename... Rest>
  void appendArgs(OSCPP::Client::Packet& packet, const std::vector<T>& first,
                  const Rest&... rest) {
    for (const auto& e : first) {
      appendArgs(packet, e);
    }
    appendArgs(packet, rest...);
  }

  template <typename T, size_t N, typename... Rest>
  void appendArgs(OSCPP::Client::Packet& packet, const std::array<T, N>& first,
                  const Rest&... rest) {
    for (const auto& e : first) {
      appendArgs(packet, e);
    }
    appendArgs(packet, rest...);
  }
};

}  // namespace net
}  // namespace limas
