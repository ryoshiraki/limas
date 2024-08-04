#pragma once
#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>
#else
#include <sys/types.h>
#include <unistd.h>  // For getpid()
#endif

#include "system/Exception.h"
#include "system/Logger.h"

namespace rs {
namespace subprocess {

inline int execute(const std::string& command, std::string* output = nullptr) {
  int exitcode = 0;
  std::array<char, 1048576> buffer{};
  std::string result;
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define WEXITSTATUS
#endif
  FILE* pipe = popen(command.c_str(), "r");

  if (pipe == nullptr) throw rs::Exception("popen() failed!");

  try {
    std::size_t bytesread;
    while ((bytesread = std::fread(buffer.data(), sizeof(buffer.at(0)),
                                   sizeof(buffer), pipe)) != 0) {
      result += std::string(buffer.data(), bytesread);
    }
  } catch (...) {
    pclose(pipe);
    throw rs::Exception("fread() failed!");
  }

  if (output) *output = result;
  int code = pclose(pipe);
  return WEXITSTATUS(code);
}

inline int getProcessId() {
#ifdef _WIN32
  int pid = GetCurrentProcessId();
#else
  int pid = getpid();
#endif
  return pid;
}

inline int forceQuit() {
  int process_id = getProcessId();
  std::string command = "kill -9 " + to_string(process_id);
  return std::system(command.c_str());
}

inline int reboot() {
  int process_id = getProcessId();
  std::string command = "open -n \"" + rs::fs::getExecutablePath() + "\"";
  command += " & kill -9 " + to_string(process_id);
  return std::system(command.c_str());
}

}  // namespace subprocess
}  // namespace rs
