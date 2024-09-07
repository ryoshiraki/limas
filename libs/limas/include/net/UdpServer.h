#pragma once
#include "system/Logger.h"

namespace limas {
namespace net {

using boost::asio::ip::udp;

class UdpServer {
  static const int MAX_BUFFER_SIZE = 1024;  // Adjust buffer size as needed
  using MessageHandler = std::function<void(const std::string&)>;

 public:
  UdpServer(int port) : socket_(io_service_, udp::endpoint(udp::v4(), port)) {
    handler_ = [](const std::string& m) {
      log::info("UdpServer") << m << log::end();
    };
    startReceive();
  }

  void run() { io_service_.run(); }
  void setCallback(const MessageHandler& handler) { handler_ = handler; }

 private:
  void startReceive() {
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&UdpServer::handleReceive, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void handleReceive(const boost::system::error_code& error,
                     std::size_t bytes_recvd) {
    if (!error || error == boost::asio::error::message_size) {
      handler_(std::string(recv_buffer_.data(), bytes_recvd));
      startReceive();
    }
  }

  boost::asio::io_service io_service_;
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, MAX_BUFFER_SIZE> recv_buffer_;
  MessageHandler handler_;
};

}  // namespace net
}  // namespace limas