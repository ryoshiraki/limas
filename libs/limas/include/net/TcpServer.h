#pragma once
#include "system/Logger.h"

namespace rs {
namespace net {

using boost::asio::ip::tcp;

class TcpServer {
  using MessageHandler = std::function<void(const std::string&)>;

 public:
  TcpServer(boost::asio::io_service& io_service, int port)
      : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
        socket_(io_service) {
    handler_ = [](const std::string& m) {
      log::info("TcpServer") << m << log::end();
    };
    startAccept();
  }

  void setCallback(const MessageHandler& handler) { handler_ = handler; }

 private:
  void startAccept() {
    acceptor_.async_accept(socket_,
                           boost::bind(&TcpServer::handleAccept, this,
                                       boost::asio::placeholders::error));
  }

  void handleAccept(const boost::system::error_code& error) {
    if (!error) {
      startRead();
      startAccept();
    }
  }

  void startRead() {
    boost::asio::async_read_until(
        socket_, buffer_, "\n",
        boost::bind(&TcpServer::handleRead, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void handleRead(const boost::system::error_code& error,
                  std::size_t bytes_transferred) {
    if (!error) {
      std::string message(boost::asio::buffer_cast<const char*>(buffer_.data()),
                          bytes_transferred);
      handler_(message);
      buffer_.consume(bytes_transferred);
      startRead();
    }
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
  boost::asio::streambuf buffer_;
  MessageHandler handler_;
};

}  // namespace net
}  // namespace rs