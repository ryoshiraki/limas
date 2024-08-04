#pragma once
#include "math/Math.h"
#include "system/Noncopyable.h"
#include "system/Singleton.h"
#include "utils/FileSystem.h"
#include "utils/Utils.h"

namespace limas {
namespace log {

static struct endlog_t {
} endl;

enum class LogLevel { INFO, WARN, ERROR };

class Logger : private Noncopyable {
 public:
  virtual ~Logger() {}

  Logger& setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mtx_);
    level_ = level;
    return *this;
  }

  Logger& info(const std::string& name = "") {
    return log(LogLevel::INFO, name);
  }

  Logger& warn(const std::string& name = "") {
    return log(LogLevel::WARN, name);
  }

  Logger& error(const std::string& name = "") {
    return log(LogLevel::ERROR, name);
  }

  template <typename T>
  Logger& operator<<(const T& msg) {
    if (current_level_ >= level_) {
      std::lock_guard<std::mutex> lock(mtx_);
      *stream_ << msg;
    }
    return *this;
  }

  Logger& operator<<(std::ios_base& (*pf)(std::ios_base&)) {
    if (current_level_ >= level_) {
      std::lock_guard<std::mutex> lock(mtx_);
      *stream_ << pf;
    }
    return *this;
  }

  virtual Logger& operator<<(std::ostream& (*pf)(std::ostream&)) {
    if (current_level_ >= level_) {
      std::lock_guard<std::mutex> lock(mtx_);
      *stream_ << pf;
    }
    return *this;
  }

  Logger& operator<<(endlog_t) { return *this << std::endl; }

 protected:
  LogLevel level_;
  LogLevel current_level_;
  std::ostream* stream_;
  mutable std::mutex mtx_;

  Logger(std::ostream* stream) : stream_(stream), level_(LogLevel::INFO) {}

  Logger& log(LogLevel level, const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx_);
    current_level_ = level;
    if (level >= level_) {
      *stream_ << std::right << std::noshowpos << std::setfill(' ')
               << std::setw(0) << "[" << getTimestamp() << "] ["
               << ((level == LogLevel::INFO)    ? "info"
                   : (level == LogLevel::WARN)  ? "warn"
                   : (level == LogLevel::ERROR) ? "error"
                                                : "unknown")
               << "] ";
      if (name.size()) {
        *stream_ << "[" << name << "] ";
      }
    }
    return *this;
  }

  std::string getTimestamp() const {
    using namespace std::chrono;
    auto now = std::chrono::system_clock::now();
    auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto fraction = now - seconds;
    auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(fraction);

    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm time_buf;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&time_buf, &time);
#else
    localtime_r(&time, &time_buf);
#endif

    std::ostringstream oss;
    oss << std::put_time(&time_buf, "%Y-%m-%d %H:%M:%S") << '.'
        << std::setfill('0') << std::setw(3) << milliseconds.count();
    return oss.str();
  }
};

class ConsoleLogger : public Logger {
  friend class Singleton<ConsoleLogger>;

 private:
  ConsoleLogger() : Logger(&std::cout) {}
};

class FileLogger : public Logger {
  static const int MAX_SIZE = 1e6;
  friend class Singleton<FileLogger>;

 public:
  ~FileLogger() {
    if (file_.is_open()) file_.close();
  }

  void openNewFile() {
    if (file_.is_open()) file_.close();

    filepath_ = fileroot_ + std::to_string(log_index_++) + "_" +
                util::getTimestamp() + ".log";
    file_.open(filepath_, std::ios_base::app);
  }

  FileLogger& operator<<(std::ostream& (*pf)(std::ostream&)) override {
    if (current_level_ >= level_) {
      std::lock_guard<std::mutex> lock(mtx_);
      if (!file_.is_open()) openNewFile();

      *stream_ << pf;

      if (pf == static_cast<std::ostream& (*)(std::ostream&)>(std::endl) ||
          pf == static_cast<std::ostream& (*)(std::ostream&)>(std::flush)) {
        if (file_.tellp() >= MAX_SIZE) {
          openNewFile();
        }
      }
    }
    return *this;
  }

  void setOutputDirectory(const std::string& dir) { fileroot_ = dir; }

 private:
  std::ofstream file_;
  std::string filepath_;
  std::string fileroot_;
  int log_index_;

  FileLogger() : Logger(&file_), log_index_(0) {
    // fileroot_ = fs::getProjectPath() + "log/";
  }
};

inline void setOutputDirectory(const std::string& dir) {
  Singleton<FileLogger>::getInstance().setOutputDirectory(dir);
}

inline void setLevel(LogLevel level) {
  Singleton<ConsoleLogger>::getInstance().setLevel(level);
  Singleton<FileLogger>::getInstance().setLevel(level);
}

inline endlog_t& end() { return endl; }

inline std::string indent(const int indent) {
  std::string str;
  for (int i = 0; i < indent; i++) {
    str += "  ";
  }
  return str;
}

inline Logger& info(const std::string& name = "") {
  return Singleton<ConsoleLogger>::getInstance().info(name);
}

inline Logger& warn(const std::string& name = "") {
  return Singleton<ConsoleLogger>::getInstance().warn(name);
}

inline Logger& error(const std::string& name = "") {
  return Singleton<ConsoleLogger>::getInstance().error(name);
}

inline Logger& infoToFile(const std::string& name = "") {
  return Singleton<FileLogger>::getInstance().info(name);
}

inline Logger& warnToFile(const std::string& name = "") {
  return Singleton<FileLogger>::getInstance().warn(name);
}

inline Logger& errorToFile(const std::string& name = "") {
  return Singleton<FileLogger>::getInstance().error(name);
}

}  // namespace log
}  // namespace limas
