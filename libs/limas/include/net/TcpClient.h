#pragma once

namespace limas {
namespace net {

using boost::asio::ip::tcp;

class TcpClient {
 public:
  TcpClient(boost::asio::io_service& io_service, const std::string& host,
            int port)
      : socket_(io_service), resolver_(io_service) {
    tcp::resolver::query query(host, std::to_string(port));
    tcp::resolver::iterator endpoint_iterator = resolver_.resolve(query);
    boost::asio::connect(socket_, endpoint_iterator);
  }

  ~TcpClient() {
    if (socket_.is_open()) {
      socket_.close();
    }
  }

  void send(const std::string& msg) {
    boost::asio::write(socket_, boost::asio::buffer(msg));
  }

  void send(const std::vector<unsigned char>& data) {
    boost::asio::write(socket_, boost::asio::buffer(data));
  }

 private:
  tcp::socket socket_;
  tcp::resolver resolver_;
};

}  // namespace net
}  // namespace limas