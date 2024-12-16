#pragma once
#include "ip/UdpSocket.h"
#include "osc/OscPacketListener.h"
#include "osc/OscReceivedElements.h"
#include "system/Logger.h"

namespace limas {
namespace net {

class OscPackReceiver : public osc::OscPacketListener {
  class BaseOscHandler {
   public:
    using Ptr = std::shared_ptr<BaseOscHandler>;
    virtual bool process(const osc::ReceivedMessage& message) = 0;
  };

  template <typename... Args>
  class OscHandler : public BaseOscHandler {
    using ArgumentTypes = std::tuple<Args...>;

    template <std::size_t... I>
    ArgumentTypes getArgs(osc::ReceivedMessage::const_iterator it,
                          std::index_sequence<I...>) {
      return std::make_tuple((getArg<Args>(it))...);
    }

    template <typename T>
    T getArg(osc::ReceivedMessage::const_iterator& it);

    template <>
    bool getArg<bool>(osc::ReceivedMessage::const_iterator& it) {
      return (it++)->AsBool();
    }

    template <>
    int getArg<int>(osc::ReceivedMessage::const_iterator& it) {
      return (it++)->AsInt32();
    }

    template <>
    long getArg<long>(osc::ReceivedMessage::const_iterator& it) {
      return (it++)->AsInt64();
    }

    template <>
    float getArg<float>(osc::ReceivedMessage::const_iterator& it) {
      return (it++)->AsFloat();
    }

    template <>
    double getArg<double>(osc::ReceivedMessage::const_iterator& it) {
      return (it++)->AsDouble();
    }

    template <>
    std::string getArg<std::string>(osc::ReceivedMessage::const_iterator& it) {
      return (it++)->AsString();
    }

   public:
    OscHandler(const std::function<void(Args...)>& handler)
        : handler_(handler) {}

    virtual bool process(const osc::ReceivedMessage& message) {
      if (message.ArgumentCount() == sizeof...(Args)) {
        auto it = message.ArgumentsBegin();
        ArgumentTypes args = getArgs(it, std::index_sequence_for<Args...>{});
        std::apply(handler_, args);
        return true;
      }
      return false;
    }

    std::function<void(Args...)> handler_;
  };

 public:
  virtual ~OscPackReceiver() {
    if (socket_) {
      socket_->Break();
      if (thread_.joinable()) {
        thread_.join();
      }
      socket_.reset();
    }
  }

  void setup(int port) {
    try {
      IpEndpointName name(IpEndpointName::ANY_ADDRESS, port);
      auto deleter = [](UdpListeningReceiveSocket* s) { s->Break(); };
      socket_ = std::unique_ptr<UdpListeningReceiveSocket, decltype(deleter)>(
          new UdpListeningReceiveSocket(name, this), deleter);
    } catch (std::exception& e) {
      logger::error("OscPackReceiver")
          << " Server: "
          << "couldn't create receiver on port " << port << logger::end();
    }

    thread_ = std::thread([this] {
      while (socket_) {
        try {
          socket_->Run();
        } catch (std::exception& e) {
          std::cerr << "Server: " << e.what() << std::endl;
        }
      }
    });

    thread_.detach();
  }

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

  void stopListening(const std::string& address) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = handlers_.find(address);
    if (it != handlers_.end()) handlers_.erase(it);
  }

 protected:
  template <typename... Args>
  void listenMessage(const std::string& address,
                     const std::function<void(Args...)>& func) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (handlers_.find(address) != handlers_.end()) {
      return;
    }
    auto handler = std::make_shared<OscHandler<Args...>>(func);
    handlers_.insert(make_pair(address, handler));
  }

  virtual void ProcessMessage(const osc::ReceivedMessage& message,
                              const IpEndpointName& endpoint) {
    (void)endpoint;  // Suppress unused parameter warning

    try {
      std::lock_guard<std::mutex> lock(mutex_);
      for (auto& handler : handlers_) {
        if (message.AddressPattern() == handler.first) {
          if (handler.second->process(message)) break;
        }
      }
    } catch (const std::exception& e) {
      std::cerr << "Error processing OSC message: " << e.what() << std::endl;
    }
  }

 private:
  std::unique_ptr<UdpListeningReceiveSocket,
                  std::function<void(UdpListeningReceiveSocket*)>>
      socket_;

  std::thread thread_;
  std::mutex mutex_;
  std::map<std::string, BaseOscHandler::Ptr> handlers_;
};

}  // namespace net
}  // namespace limas

// #pragma once
// #include <stdexcept>

// #include "ip/UdpSocket.h"
// #include "osc/OscPacketListener.h"
// #include "osc/OscReceivedElements.h"
// #include "system/Logger.h"

// namespace limas {
// namespace net {

// class OscReceiver : public osc::OscPacketListener {
//   class BaseOscHandler {
//    public:
//     using Ptr = std::shared_ptr<BaseOscHandler>;
//     virtual bool process(const osc::ReceivedMessage& message) = 0;
//   };

//   template <typename... Args>
//   class OscHandler : public BaseOscHandler {
//     using ArgumentTypes = std::tuple<Args...>;

//     template <std::size_t... I>
//     ArgumentTypes getArgs(osc::ReceivedMessage::const_iterator it,
//                           std::index_sequence<I...>) {
//       return std::make_tuple((getArg<Args>(it))...);
//     }

//     template <typename T>
//     T getArg(osc::ReceivedMessage::const_iterator& it) {
//       throw std::runtime_error("Unsupported argument type");
//     }

//     template <>
//     bool getArg<bool>(osc::ReceivedMessage::const_iterator& it) {
//       return (it++)->AsBool();
//     }

//     template <>
//     int getArg<int>(osc::ReceivedMessage::const_iterator& it) {
//       return (it++)->AsInt32();
//     }

//     template <>
//     long getArg<long>(osc::ReceivedMessage::const_iterator& it) {
//       return (it++)->AsInt64();
//     }

//     template <>
//     float getArg<float>(osc::ReceivedMessage::const_iterator& it) {
//       return (it++)->AsFloat();
//     }

//     template <>
//     double getArg<double>(osc::ReceivedMessage::const_iterator& it) {
//       return (it++)->AsDouble();
//     }

//     template <>
//     std::string getArg<std::string>(osc::ReceivedMessage::const_iterator& it)
//     {
//       return (it++)->AsString();
//     }

//    public:
//     OscHandler(const std::function<void(Args...)>& handler)
//         : handler_(handler) {}

//     virtual bool process(const osc::ReceivedMessage& message) override {
//       if (message.ArgumentCount() == sizeof...(Args)) {
//         auto it = message.ArgumentsBegin();
//         try {
//           ArgumentTypes args = getArgs(it,
//           std::index_sequence_for<Args...>{}); std::apply(handler_, args);
//           return true;
//         } catch (const std::exception& e) {
//           std::cerr << "Argument type mismatch: " << e.what() << std::endl;
//           return false;
//         }
//       }
//       return false;
//     }

//    private:
//     std::function<void(Args...)> handler_;
//   };

//  public:
//   virtual ~OscReceiver() {
//     if (socket_) socket_.reset();
//     if (thread_.joinable()) thread_.join();
//   }

//   // void setup(int port) {
//   //   try {
//   //     IpEndpointName name(IpEndpointName::ANY_ADDRESS, port);
//   //     auto deleter = [](UdpListeningReceiveSocket* s) { s->Break(); };
//   //     socket_ = std::unique_ptr<UdpListeningReceiveSocket,
//   //     decltype(deleter)>(
//   //         new UdpListeningReceiveSocket(name, this), deleter);
//   //   } catch (std::exception& e) {
//   //     logger::error("OscReceiver")
//   //         << " Server: "
//   //         << "couldn't create receiver on port " << port << logger::end();
//   //   }

//   //   thread_ = std::thread([this] {
//   //     try {
//   //       while (socket_) {
//   //         socket_->Run();
//   //       }
//   //     } catch (std::exception& e) {
//   //       std::cerr << "Server error: " << e.what() << std::endl;
//   //     }
//   //   });
//   // }

//   void setup(int port) {
//     try {
//       IpEndpointName name(IpEndpointName::ANY_ADDRESS, port);
//       auto deleter = [](UdpListeningReceiveSocket* s) { s->Break(); };
//       socket_ = std::unique_ptr<UdpListeningReceiveSocket,
//       decltype(deleter)>(
//           new UdpListeningReceiveSocket(name, this), deleter);
//     } catch (std::exception& e) {
//       logger::error("OscReceiver")
//           << " Server: "
//           << "couldn't create receiver on port " << port << logger::end();
//     }

//     thread_ = std::thread([this] {
//       while (socket_) {
//         try {
//           socket_->Run();
//         } catch (std::exception& e) {
//           std::cerr << "Server: " << e.what() << std::endl;
//         }
//       }
//     });

//     thread_.detach();
//   }

//   template <typename... Args>
//   void listen(const std::string& address,
//               const std::function<void(Args...)>& func) {
//     listenMessage<Args...>(address, func);
//   }

//   template <typename Func>
//   inline void listen(const std::string& address, Func&& func) {
//     listenMessage(address, std::function(std::forward<Func>(func)));
//   }

//   void stopListening(const std::string& address) {
//     std::lock_guard<std::mutex> lock(mutex_);
//     handlers_.erase(address);
//   }

//  protected:
//   template <typename... Args>
//   void listenMessage(const std::string& address,
//                      const std::function<void(Args...)>& func) {
//     std::lock_guard<std::mutex> lock(mutex_);
//     handlers_.emplace(address, std::make_shared<OscHandler<Args...>>(func));
//   }

//   virtual void ProcessMessage(const osc::ReceivedMessage& message,
//                               const IpEndpointName& endpoint) override {
//     (void)endpoint;

//     std::lock_guard<std::mutex> lock(mutex_);
//     auto it = handlers_.find(message.AddressPattern());
//     if (it != handlers_.end() && it->second->process(message)) {
//       return;
//     }

//     logger::error("OscPackReceiver")
//         << "No handler found for address: " << message.AddressPattern()
//         << logger::end();
//   }

//  private:
//   std::unique_ptr<UdpListeningReceiveSocket,
//                   std::function<void(UdpListeningReceiveSocket*)>>
//       socket_;
//   std::thread thread_;
//   std::mutex mutex_;
//   std::map<std::string, BaseOscHandler::Ptr> handlers_;
// };

// }  // namespace net
// }  // namespace limas
