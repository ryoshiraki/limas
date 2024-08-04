#pragma once
#include "system/Noncopyable.h"

namespace rs {

class Connection {
 public:
  Connection() {}
  Connection(const boost::signals2::connection conn) : conn_(conn) {}
  bool isConnected() const { return conn_.connected(); }
  void disconnect() {
    if (isConnected()) conn_.disconnect();
  }

 private:
  boost::signals2::connection conn_;
};

template <typename ArgumentsType>
class Observable : private Noncopyable {
 public:
  using ObserverFunction = void(ArgumentsType&);
  using Signal = boost::signals2::signal<ObserverFunction>;

  template <class ObserverClass>
  Connection addObserver(ObserverClass* listener,
                         void (ObserverClass::*callback)(ArgumentsType&)) {
    auto f = std::bind(callback, listener, std::placeholders::_1);
    return Connection(addObserver(f));
  }

  Connection addObserver(const std::function<ObserverFunction>& callback) {
    return Connection(signal_.connect(callback));
  }

  void notify(ArgumentsType& args) { signal_(args); }

 private:
  Signal signal_;
};

class ObservableNoArgs : private Noncopyable {
 public:
  using ObserverFunction = void();
  using Signal = boost::signals2::signal<ObserverFunction>;

  Connection addObserver(const std::function<ObserverFunction>& callback) {
    return Connection(signal_.connect(callback));
  }

  void notify() { signal_(); }

 private:
  Signal signal_;
};

struct EventArgs {
  // empty
};

struct KeyEventArgs {
  int key;
};

enum {
  MOUSE_BUTTON_NONE = -1,
  MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT,
  MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
};

struct MouseEventArgs {
  double x;
  double y;
  int button;
};

struct ScrollEventArgs {
  double x;
  double y;
};

struct ResizeEventArgs {
  int width;
  int height;
};

struct FileDropEventArgs {
  std::vector<std::string> paths;
};

struct CoreObservable {
  Observable<const EventArgs> loop_begin;
  Observable<const EventArgs> loop_end;

  Observable<const KeyEventArgs> key_pressed;
  Observable<const KeyEventArgs> key_released;

  Observable<const MouseEventArgs> mouse_moved;
  Observable<const MouseEventArgs> mouse_pressed;
  Observable<const MouseEventArgs> mouse_released;

  Observable<const ScrollEventArgs> mouse_scrolled;

  Observable<const ResizeEventArgs> window_resized;
  Observable<const EventArgs> window_refreshed;
  Observable<const EventArgs> window_closed;

  Observable<const FileDropEventArgs> file_dropped;
};

}  // namespace rs
