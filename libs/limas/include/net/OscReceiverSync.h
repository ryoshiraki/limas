#pragma once
#include <mutex>
#include <queue>

#include "net/UdpServer.h"
#include "oscpp/print.hpp"
#include "oscpp/server.hpp"
#include "system/Exception.h"
#include "system/Thread.h"

namespace limas {
namespace net {

class OscReceiverSync {
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
  OscReceiverSync() {}

  void setup(uint16_t port) {
    server_ = std::make_unique<net::UdpServer>(port);
    port_ = port;

    server_->start([&](const std::string& buffer) {
      std::lock_guard<std::mutex> lock(mutex_);
      message_queue_.push(buffer);  // メッセージをキューに格納
    });
  }

  uint16_t getPort() const { return port_; }

  void update() {
    std::queue<std::string> local_queue;

    {
      std::lock_guard<std::mutex> lock(mutex_);
      std::swap(local_queue, message_queue_);
    }

    while (!local_queue.empty()) {
      std::string buffer = local_queue.front();
      local_queue.pop();

      OSCPP::Server::Packet packet(buffer.data(), buffer.size());
      try {
        handlePacket(packet);
      } catch (const OSCPP::ParseError& e) {
        logger::error("OSC Receiver") << e.what() << logger::end();
      } catch (const std::exception& e) {
        logger::error("OSC Receiver") << e.what() << logger::end();
      }
    }
  }

  template <typename... Args>
  void listen(const std::string& address, void (*func)(Args...)) {
    listenMessage<Args...>(address, func);
  }

  template <typename... Args>
  void listen(const std::string& address,
              const std::function<void(Args...)>& func) {
    listenMessage<Args...>(address, func);
  }

  template <typename Class, typename... Args>
  void listen(const std::string& address, Class* instance,
              void (Class::*func)(Args...)) {
    listenMessage<Args...>(address, [instance, func](Args... args) {
      (instance->*func)(std::forward<Args>(args)...);
    });
  }

  template <typename Class, typename... Args>
  void listen(const std::string& address, const Class* instance,
              void (Class::*func)(Args...) const) {
    listenMessage<Args...>(address, [instance, func](Args... args) {
      (instance->*func)(std::forward<Args>(args)...);
    });
  }

  template <typename Func>
  void listen(const std::string& address, Func&& func) {
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
          logger::error("OSC Receiver")
              << "failed to handle packet" << logger::end();
        }
      }
    }
  }

  template <typename... Args>
  void listenMessage(const std::string& address,
                     const std::function<void(Args...)>& func) {
    if (handlers_.find(address) != handlers_.end()) {
      return;
    }

    try {
      auto handler = std::make_unique<OscHandler<Args...>>(func);
      handlers_.emplace(address, std::move(handler));
    } catch (const std::exception& e) {
      std::cerr << "Error while adding handler: " << e.what() << std::endl;
    }
  }

  std::unique_ptr<net::UdpServer> server_;
  uint16_t port_;
  std::map<std::string, std::unique_ptr<BaseOscHandler>> handlers_;

  std::queue<std::string> message_queue_;  // メッセージキューを追加
  std::mutex mutex_;  // キューへのアクセスを保護するためのミューテックス
};

}  // namespace net
}  // namespace limas
