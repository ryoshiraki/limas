#include <CoreFoundation/CoreFoundation.h>

#include <algorithm>

#include "system/Observable.h"

namespace rs {
class RSSyphonServerDirectory;
class RSSyphonServerDescription {
 public:
  RSSyphonServerDescription() { server_name_ = app_name_ = "null"; }

  RSSyphonServerDescription(const std::string &server_name,
                            const std::string &app_name) {
    server_name_ = server_name;
    app_name_ = app_name;
  }

  friend bool operator==(const RSSyphonServerDescription &lhs,
                         const RSSyphonServerDescription &rhs) {
    return (lhs.server_name_ == rhs.server_name_) &&
           (lhs.app_name_ == rhs.app_name_);
  }

  std::string server_name_, app_name_;
};

class RSSyphonServerDirectoryEventArgs {
 public:
  std::vector<RSSyphonServerDescription> servers_;
};

class RSSyphonServerDirectoryEvents {
 public:
  Observable<RSSyphonServerDirectoryEventArgs> server_updated_;
  Observable<RSSyphonServerDirectoryEventArgs> server_announced_;
  Observable<RSSyphonServerDirectoryEventArgs> server_retired_;
};

class RSSyphonServerDirectory {
 public:
  RSSyphonServerDirectory();
  ~RSSyphonServerDirectory();

  bool serverExists(const std::string &server_name,
                    const std::string &app_name);
  bool serverExists(const RSSyphonServerDescription &_server);
  const RSSyphonServerDescription &getDescription(int _idx);

  const vector<RSSyphonServerDescription> &getServerList();
  RSSyphonServerDirectoryEvents events_;

  // copy and assignment are not supported
  RSSyphonServerDirectory(const RSSyphonServerDirectory &o) = delete;
  RSSyphonServerDirectory &operator=(const RSSyphonServerDirectory &o) = delete;

  // needs to be public because of the nature of CFNotifications.  please do not
  // call this.
  void handleNotification(CFStringRef name, CFDictionaryRef userInfo);

 private:
  // void update(ofEventArgs &args);
  void refresh(bool isAnnounce);
  void serverAnnounced();
  void serverUpdated();
  void serverRetired();

  void addObservers();
  void removeObservers();

  bool CFStringRefToString(CFStringRef src, std::string &dest);

  std::vector<RSSyphonServerDescription> servers_;
};
}  // namespace rs