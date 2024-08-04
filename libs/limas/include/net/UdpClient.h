#pragma once

namespace limas {
namespace net {

using boost::asio::ip::udp;

class UdpClient {
 public:
  UdpClient(boost::asio::io_service& io_service, const std::string& host,
            int port)
      : socket_(io_service),
        endpoint_(*udp::resolver{io_service}
                       .resolve(udp::v4(), host, std::to_string(port))
                       .begin()) {
    socket_.open(udp::v4());
  }

  ~UdpClient() {
    if (socket_.is_open()) {
      socket_.close();
    }
  }

  void send(const std::string& msg) {
    socket_.send_to(boost::asio::buffer(msg), endpoint_);
  }

  void send(const std::vector<unsigned char>& data) {
    socket_.send_to(boost::asio::buffer(data), endpoint_);
  }

 private:
  udp::socket socket_;
  udp::endpoint endpoint_;
};

}  // namespace net
}  // namespace limas