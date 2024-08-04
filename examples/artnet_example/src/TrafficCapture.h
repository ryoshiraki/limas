#pragma once
#include <netinet/ip.h>
#include <netinet/udp.h>

#include "pcap.h"
#include "system/Thread.h"

namespace rs {
namespace net {

class TrafficCapture : public Thread {
 protected:
  pcap_t* handle_;

  virtual void processPacket(const struct pcap_pkthdr* pkthdr,
                             const unsigned char* packet) = 0;

  static void packetHandler(unsigned char* user_data,
                            const struct pcap_pkthdr* pkthdr,
                            const unsigned char* packet) {
    TrafficCapture* instance = reinterpret_cast<TrafficCapture*>(user_data);
    auto locker = instance->getLock();
    instance->processPacket(pkthdr, packet);
  }

 public:
  TrafficCapture() {}
  virtual ~TrafficCapture() { pcap_close(handle_); }

  void start(const char* device, const char* filter_exp, int timeout) {
    char errbuf[PCAP_ERRBUF_SIZE];
    handle_ = pcap_open_live(device, BUFSIZ, 1, timeout, errbuf);
    if (!handle_) {
      log::error("TrafficCapture")
          << "Could not open device " << device << ": " << errbuf << log::end();
      return;
    }

    struct bpf_program filter;
    if (pcap_compile(handle_, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) ==
        -1) {
      log::error("TrafficCapture") << "Could not parse filter" << log::end();
      return;
    }
    if (pcap_setfilter(handle_, &filter) == -1) {
      log::error("TrafficCapture") << "Could not install filter" << log::end();
      return;
    }

    startThread();
  }

 private:
  void threadedFunction() {
    pcap_loop(handle_, 0, TrafficCapture::packetHandler,
              reinterpret_cast<unsigned char*>(this));

    while (isThreadRunning()) {
    }
  }
};

}  // namespace net
}  // namespace rs