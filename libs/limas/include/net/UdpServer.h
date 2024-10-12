#pragma once
#include "system/Logger.h"
#include "system/Thread.h"

namespace limas {
namespace net {

using boost::asio::ip::udp;

class UdpServer : public Thread {
  static const int MAX_BUFFER_SIZE = 8192;  // Adjust buffer size as needed
  using MessageHandler = std::function<void(const std::string&)>;

 public:
  UdpServer(int port)
      : io_context_(), socket_(io_context_, udp::endpoint(udp::v4(), port)) {
    handler_ = [](const std::string& m) {
      log::info("UdpServer") << m << log::end();
    };
    startReceive();
  }

  void start(const MessageHandler& handler) {
    handler_ = handler;
    startThread([this]() { io_context_.run(); });
  }

  void stop() {
    io_context_.stop();
    stopThread();
  }

 private:
  void startReceive() {
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        [this](boost::system::error_code ec, std::size_t bytes_recvd) {
          if (!ec && bytes_recvd > 0) {
            handleReceive(recv_buffer_, bytes_recvd);
          }
          startReceive();
        });
  }

  void handleReceive(char data[], std::size_t length) {
    std::string message(data, length);
    handler_(message);
  }

  boost::asio::io_context io_context_;
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  char recv_buffer_[MAX_BUFFER_SIZE];
  MessageHandler handler_;
};

}  // namespace net
}  // namespace limas