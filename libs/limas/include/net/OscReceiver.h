#pragma once
#include "net/UdpServer.h"
#include "oscpp/print.hpp"
#include "oscpp/server.hpp"
#include "system/Exception.h"
#include "system/Thread.h"

namespace limas {
namespace net {

class OscReceiver : public Thread {
  class BaseOscHandler {
   public:
    virtual bool handle(OSCPP::Server::ArgStream& args) = 0;
  };

  template <typename... Args>
  class OscHandler : public BaseOscHandler {
    using ArgumentTypes = std::tuple<Args...>;

    template <std::size_t... I>
    ArgumentTypes getArgs(OSCPP::Server::ArgStream& args,
                          std::index_sequence<I...>) {
      return std::make_tuple((getArg<Args>(args))...);
    }

    template <typename T>
    T getArg(OSCPP::Server::ArgStream& args);

    template <>
    int32_t getArg<int32_t>(OSCPP::Server::ArgStream& args) {
      return args.int32();
    }

    template <>
    float getArg<float>(OSCPP::Server::ArgStream& args) {
      return args.float32();
    }

    template <>
    std::string getArg<std::string>(OSCPP::Server::ArgStream& args) {
      return args.string();
    }

    template <>
    bool getArg<bool>(OSCPP::Server::ArgStream& args) {
      return args.int32();
    }

   public:
    OscHandler(std::function<void(Args...)> handler)
        : handler_(std::move(handler)) {}

    virtual bool handle(OSCPP::Server::ArgStream& args) override {
      if (args.size() == sizeof...(Args)) {
        ArgumentTypes arg_tuple =
            getArgs(args, std::index_sequence_for<Args...>{});
        std::apply(handler_, arg_tuple);
        return true;
      }
      return false;
    }

    std::function<void(Args...)> handler_;
  };

 public:
  OscReceiver() {}

  void setup(uint16_t port = 6454) {
    server_ = std::make_unique<net::UdpServer>(port);
    port_ = port;

    server_->setCallback([&](const string& buffer) {
      OSCPP::Server::Packet packet =
          OSCPP::Server::Packet(buffer.data(), buffer.size());
      try {
        handlePacket(packet);
      } catch (const OSCPP::ParseError& e) {
        log::error("OSC Receiver") << e.what() << log::end();
      } catch (const std::exception& e) {
        log::error("OSC Receiver") << e.what() << log::end();
      }
    });
    startThread();
  }
  uint16_t getPort() const { return port_; }

  // 特殊化版: 関数ポインタ
  template <typename... Args>
  void listen(const std::string& address, void (*func)(Args...)) {
    listenMessage<Args...>(address, func);
  };

  // 特殊化版: std::function
  template <typename... Args>
  inline void listen(const std::string& address,
                     const std::function<void(Args...)>& func) {
    listenMessage<Args...>(address, func);
  };

  // 特殊化版:
  // メンバ関数ポインタ（メンバ関数ポインタは通常の関数とは異なる型なので別途特殊化が必要）
  template <typename Class, typename... Args>
  inline void listen(const std::string& address, void (Class::*func)(Args...)) {
    listenMessage<Args...>(address, func);
  };

  // メンバ関数ポインタのconst版
  template <typename Class, typename... Args>
  inline void listen(const std::string& address,
                     void (Class::*func)(Args...) const) {
    listenMessage<Args...>(address, func);
  };

  // ユーザー定義の関数オブジェクト（ラムダ式やfunctor）のための特殊化
  // operator()を持つクラスに対する特殊化
  template <typename Func>
  inline void listen(const std::string& address, Func&& func) {
    listenMessage(address, std::function(func));
  }

 protected:
  void handlePacket(const OSCPP::Server::Packet& packet) {
    if (packet.isBundle()) {
      OSCPP::Server::Bundle bundle(packet);
      OSCPP::Server::PacketStream packets(bundle.packets());
      while (!packets.atEnd()) {
        handlePacket(packets.next());
      }
    } else {
      OSCPP::Server::Message msg(packet);
      OSCPP::Server::ArgStream args(msg.args());
      auto it = handlers_.find(msg.address());
      if (it != handlers_.end()) {
        if (!it->second->handle(args)) {
          log::error("OSC Receiver") << "failed to handle packet" << log::end();
        }
      }
    }
  }

  template <typename... Args>
  void listenMessage(const std::string& address,
                     const std::function<void(Args...)>& func) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (handlers_.find(address) != handlers_.end()) {
      return;
    }

    try {
      auto handler = std::make_unique<OscHandler<Args...>>(func);
      handlers_.insert(std::make_pair(address, std::move(handler)));
    } catch (const std::exception& e) {
      std::cerr << "Error while adding handler: " << e.what() << std::endl;
    }
  }

  std::unique_ptr<net::UdpServer> server_;
  uint16_t port_;
  std::map<std::string, std::unique_ptr<BaseOscHandler>> handlers_;

 private:
  void threadedFunction() {
    server_->run();
    while (isThreadRunning()) {
    }
  }
};

}  // namespace net
}  // namespace limas
