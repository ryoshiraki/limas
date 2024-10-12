#pragma once
#include <CoreFoundation/CoreFoundation.h>

#include "system/Logger.h"
#include "utils/Utils.h"

namespace limas {
namespace fs {

using namespace std;

inline string getStem(const string& filepath) {
  return filesystem::path(filepath).stem().string();
}

inline string getExtension(const string& filepath) {
  return filesystem::path(filepath).extension().string();
}

inline string getParent(const string& filepath) {
  return filesystem::path(filepath).parent_path().string();
}

inline bool isFile(const string& path) {
  return filesystem::is_regular_file(path);
}

inline bool isDirectory(const string& path) {
  return filesystem::is_directory(path);
}

inline std::time_t getLastWriteTime(const string& path) {
  filesystem::file_time_type tp = filesystem::last_write_time(path);
  namespace chrono = std::chrono;
  auto sec = chrono::duration_cast<chrono::seconds>(tp.time_since_epoch());
  std::time_t t = sec.count();
  return t;
}

inline vector<string> getDirectories(const string& path) {
  vector<string> dirs;
  for (const auto& entry : filesystem::directory_iterator(path)) {
    if (isDirectory(entry.path())) {
      dirs.push_back(entry.path().string());
    }
  }
  return dirs;
}

inline vector<string> getFiles(const string& path, const string& ext = "") {
  vector<string> dirs;
  for (const auto& entry : filesystem::directory_iterator(path)) {
    if (isFile(entry.path())) {
      if (ext.empty() || getExtension(entry.path()) == ext) {
        dirs.push_back(entry.path().string());
      }
    }
  }
  return dirs;
}

inline vector<string> getFilesRecursive(const string& path,
                                        const string& ext = "") {
  vector<string> dirs;
  for (const filesystem::directory_entry& entry :
       filesystem::recursive_directory_iterator(path)) {
    if (isFile(entry.path())) {
      if (ext.empty() || getExtension(entry.path()) == ext) {
        dirs.push_back(entry.path().string());
      }
    }
  }
  return dirs;
}

inline bool isExists(const filesystem::path& p,
                     filesystem::file_status s = filesystem::file_status{}) {
  if (filesystem::status_known(s) ? filesystem::exists(s)
                                  : filesystem::exists(p))
    return true;
  return false;
}

#if defined(__APPLE__)
inline string getHomePath() { return string(getenv("HOME")); }
#endif

inline std::string getExecutablePath() {
  std::string result;

#if defined(__APPLE__)
  CFBundleRef bundle = CFBundleGetMainBundle();
  if (bundle) {
    CFURLRef url = CFBundleCopyBundleURL(bundle);
    if (url) {
      char path[PATH_MAX];
      if (CFURLGetFileSystemRepresentation(url, TRUE, (UInt8*)path, PATH_MAX)) {
        result = path;
      }
      CFRelease(url);
    }
  }
#else
#endif

  return result;
}

inline std::string getExecutableRootPath() {
  filesystem::path path = getExecutablePath();
  return path.parent_path().string() + "/";
}

inline std::string getProjectPath() {
  filesystem::path path = getExecutableRootPath();
  return path.parent_path().parent_path().string() + "/";
}

inline std::string getFrameworkPath() {
#ifdef FRAMEWORK_PATH
  return std::string(FRAMEWORK_PATH) + "/";
#else
  filesystem::path path = getProjectPath();
  while (path.stem() != "limas") {
    path = path.parent_path();
  }
  return path.string() + "/";
#endif
}

inline std::string getAssetsPath(const std::string& path = "") {
  return getProjectPath() + "assets/" + path;
}

inline std::string getResourcesPath(const std::string& path = "") {
#if defined(__APPLE__)
  std::string result;
  CFBundleRef bundle = CFBundleGetMainBundle();
  if (bundle) {
    CFURLRef resource_url = CFBundleCopyResourcesDirectoryURL(bundle);
    if (resource_url) {
      char path[PATH_MAX];
      if (CFURLGetFileSystemRepresentation(resource_url, TRUE, (UInt8*)path,
                                           PATH_MAX)) {
        result = path;
      }
      CFRelease(resource_url);
    }
  }
  return result + "/" + path;
#else
  return getProjectPath() + "resources/" + path;
#endif
}

inline std::string getCommonAssetsPath(const std::string& path = "") {
  return getFrameworkPath() + "assets/" + path;
}

inline std::string getCommonResourcesPath(const std::string& path = "") {
  return getResourcesPath("common/" + path);
}

inline bool createDirectory(const string& path) {
  return filesystem::create_directory(path);
}

inline bool removeDirectory(const string& path) {
  return isDirectory(path) ? filesystem::remove(path) : false;
}

inline bool removeFile(const string& path) {
  return isFile(path) ? filesystem::remove(path) : false;
}

inline bool copy(const string& from, const string& to) {
  filesystem::copy(from, to, filesystem::copy_options::none);
  if (isExists(from) && isExists(to)) return true;
  return false;
}

inline void sortPathsByDict(vector<string>& paths) {
  sort(paths.begin(), paths.end(), [](string& a, string& b) {
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
      if (std::isdigit(a[i]) && std::isdigit(b[j])) {
        size_t i_start = i, j_start = j;
        while (i < a.size() && std::isdigit(a[i])) ++i;
        while (j < b.size() && std::isdigit(b[j])) ++j;

        int num_a = std::stoi(a.substr(i_start, i - i_start));
        int num_b = std::stoi(b.substr(j_start, j - j_start));

        if (num_a != num_b) {
          return num_a < num_b;
        }
      } else {
        if (a[i] != b[j]) {
          return a[i] < b[j];
        }
        ++i;
        ++j;
      }
    }

    return a.size() < b.size();
  });
}

inline vector<string> getSortedPathsByDict(vector<string>& files) {
  vector<string> res = files;
  sortPathsByDict(res);
  return res;
}

inline string readFile(const string& path) {
  std::ifstream ifs(path);
  std::stringstream buffer;
  buffer << ifs.rdbuf();
  return buffer.str();
}

inline vector<string> readFileLineByLine(const string& path) {
  vector<string> lines;
  ifstream file(path);
  if (file.is_open()) {
    string line;
    while (getline(file, line)) {
      lines.push_back(line);
    }
  }
  return lines;
}

inline vector<vector<string> > loadCsv(const string& path) {
  vector<vector<string> > rows;
  auto lines = readFileLineByLine(path);
  rows.reserve(lines.size());
  for (auto& l : lines) {
    auto elems = util::getSplit(l, ",");
    rows.push_back(elems);
  }
  return rows;
}

inline void saveCsv(const string& path, const vector<vector<string> >& rows) {
  ofstream ofs(path);
  for (auto& row : rows) {
    for (auto& ele : row) ofs << ele << ',';
    ofs << "\n";
  }
  ofs << endl;
}

}  // namespace fs
}  // namespace limas
