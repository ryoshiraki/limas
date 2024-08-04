#pragma once
#include "system/SharedPtr.h"

namespace rs {
namespace net {

using boost::asio::ip::tcp;

class HttpClient : public SharedPtr<HttpClient>,
                   public std::enable_shared_from_this<HttpClient> {
 public:
  HttpClient(boost::asio::io_service& io_service, const std::string& server)
      : server_(server),
        resolver_(io_service),
        socket_(io_service),
        b_connected(false) {
    resolver_.async_resolve(
        tcp::resolver::query(server, "http"),
        [this](boost::system::error_code ec,
               tcp::resolver::iterator endpoint_iterator) {
          if (!ec) {
            boost::asio::async_connect(
                socket_, endpoint_iterator,
                [this](boost::system::error_code ec, tcp::resolver::iterator) {
                  if (!ec) {
                    b_connected = true;
                  }
                });
          }
        });
  }

  ~HttpClient() { socket_.close(); }

  void requestGet(const std::string& path) { request("GET", path, ""); }

  void requestPost(const std::string& path, const std::string& body) {
    request("POST", path, body);
  }

  void request(const std::string& method, const std::string& path,
               const std::string& body) {
    if (!b_connected) return;

    std::ostream request_stream(&request_);
    request_stream << method << " " << path << " HTTP/1.0\r\n";
    request_stream << "Host: " << server_ << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Length: " << body.length() << "\r\n";
    request_stream << "Connection: close\r\n\r\n";
    request_stream << body;

    std::cout << "request" << std::endl;

    boost::asio::async_write(
        socket_, request_,
        [this](const boost::system::error_code& ec, std::size_t) {
          if (!ec) {
            // Read the response status line.
            boost::asio::async_read_until(
                socket_, response_, "\r\n",
                [this](const boost::system::error_code& ec, std::size_t) {
                  if (!ec) {
                    // Write the response to stdout.
                    std::cout << &response_;
                  }
                });
          }
        });

    // boost::asio::write(socket_, request_);
    // boost::asio::streambuf response;
    // boost::asio::read_until(socket_, response, "\r\n");

    // std::istream is(&response);
    // std::string str(std::istreambuf_iterator<char>(is), {});
  }

 private:
  tcp::resolver resolver_;
  tcp::socket socket_;
  boost::asio::streambuf request_;
  boost::asio::streambuf response_;
  std::string server_;
  bool b_connected;
};

}  // namespace net
}  // namespace rs