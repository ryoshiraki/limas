// #pragma once
// #include "httplib.h"
// #include <mutex>
//
// namespace limas {
// namespace http {
//
// using namespace std;
//
// class Server {
// public:
//
//   void setup(const string & ip, int port) {
//     server.listen(ip, port);
//   }
//
//   void stop() {
//     server.stop();
//   }
//
//   void get(const string& path, const string& content) {
//     server.Get(path, [](const httplib::Request& req, httplib::Response& res)
//     {
//       res.set_content(content, "text/plain");
//     });
//   }
//
// protected:
//   httplib::Server server;
// };
//
// }
// }
