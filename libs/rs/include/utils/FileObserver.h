#pragma once
#include "system/Exception.h"
#include "system/Noncopyable.h"
#include "system/Observable.h"
#include "utils/FileSystem.h"

namespace rs {

class FileObserver {
 protected:
  class ObservableItem {
   public:
    using Ptr = std::shared_ptr<ObservableItem>;

    ObservableItem(const std::string& path) : path_(path) {}
    ~ObservableItem() {
      if (conn_.isConnected()) conn_.disconnect();
    }
    void setCallback(const std::function<void(const std::string&)>& callback) {
      last_time_ = fs::getLastWriteTime(path_);
      conn_ = updated_.addObserver(callback);
    }

    template <class ObserverClass>
    void setCallback(ObserverClass* observer,
                     void (ObserverClass::*callback)(const std::string&)) {
      last_time_ = fs::getLastWriteTime(path_);
      conn_ = updated_.addObserver(observer, callback);
    }

    void unsetCallback() { conn_.disconnect(); }

    void check() {
      time_t time = fs::getLastWriteTime(path_);
      if (time != last_time_) {
        updated_.notify(path_);
        last_time_ = time;
      }
    }

   private:
    ObservableItem() {}
    std::string path_;
    std::time_t last_time_;
    Connection conn_;
    Observable<const std::string> updated_;
  };

 public:
  FileObserver() {}

  void check() {
    for (auto& w : watches_) {
      w.second->check();
    }
  }

  template <class ObserverClass>
  void watch(const std::string& path, ObserverClass* observer,
             void (ObserverClass::*callback)(const std::string&)) {
    auto f = std::bind(callback, observer, std::placeholders::_1);
    watch(path, f);
  }

  void watch(const std::string& path,
             const std::function<void(const std::string&)>& callback) {
    if (!fs::isExists(path)) {
      throw rs::Exception(path + " does not exist");
      return;
    }

    auto it = watches_.find(path);
    if (it != watches_.end()) {
      it->second->unsetCallback();
      it->second->setCallback(callback);
    } else {
      auto watch = std::make_shared<ObservableItem>(path);
      watch->setCallback(callback);
      watches_.insert(std::make_pair(path, watch));
    }
  }

  void stopWatching(const std::string& path) {
    auto it = watches_.find(path);
    if (it != watches_.end()) {
      it->second->unsetCallback();
      watches_.erase(it);
    }
  }

 protected:
  std::map<std::string, ObservableItem::Ptr> watches_;
};

}  // namespace rs