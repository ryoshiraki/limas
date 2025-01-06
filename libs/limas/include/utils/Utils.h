#pragma once
#include "system/Exception.h"

namespace limas {
namespace utils {

using namespace std;

template <class T>
inline void print(const T& o) {
  cout << o << endl;
}

inline void sleepInMicroseconds(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

inline string getTimestamp() {
  auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
  stringstream ss;
  ss << put_time(localtime(&now), "%Y%m%d-%H%M%S");
  return ss.str();
}

inline string getUUID() {
  boost::uuids::random_generator generator;
  boost::uuids::uuid uuid = generator();
  return boost::uuids::to_string(uuid);
}

template <typename T>
inline string toString(const T& value, int width, char fill) {
  ostringstream oss;
  oss << fixed << setfill(fill) << setw(width) << value;
  return oss.str();
}

template <typename T>
inline string toString(const T& value, int precision, int width, char fill) {
  ostringstream oss;
  oss << fixed << setfill(fill) << setw(width) << setprecision(precision)
      << value;
  return oss.str();
}

template <typename T>
T to(const std::string& str) {
  T x;
  std::istringstream cur(str);
  cur >> x;

  if (cur.fail()) {
    string type = typeid(T).name();
    throw Exception("Failed to convert string to the " + type + ": " + str);
  }

  return x;
}

inline float toFloat(const std::string& str) { return to<float>(str); }
inline double toDouble(const std::string& str) { return to<double>(str); }
inline int toInt(const std::string& str) { return to<int>(str); }
inline int64_t toInt64(const std::string& str) { return to<int64_t>(str); }
inline char toChar(const std::string& str) { return to<char>(str); }

inline string toUpper(const string& src) {
  string dst;
  dst.resize(src.size());
  transform(src.cbegin(), src.cend(), dst.begin(), ::toupper);
  return dst;
}

inline string toLower(const string& src) {
  string dst;
  dst.resize(src.size());
  transform(src.cbegin(), src.cend(), dst.begin(), ::tolower);
  return dst;
}

inline string removeChar(std::string& str, char ch) {
  std::string clean_str = str;
  clean_str.erase(std::remove(clean_str.begin(), clean_str.end(), ch),
                  clean_str.end());
  return clean_str;
}

inline string replace(const string& s, const string& from, const string& to) {
  string r = s;
  int p = 0;
  while ((p = (int)r.find(from, p)) != string::npos) {
    r.replace(p, from.length(), to);
    p += (int)to.length();
  }
  return r;
}

inline string substring(const string& input, const uint start,
                        uint length = INT_MAX) {
  return input.substr(start, min(length, (uint)input.length() - start));
}

template <class T>
inline bool contains(const vector<T>& v, const T& match) {
  return find(v.begin(), v.end(), match) != v.end();
}

template <class T>
inline void removeIf(vector<T>& src, function<bool(T&)> func) {
  src.erase(remove_if(begin(src), end(src), func), src.end());
}

inline vector<string> getSplit(const string& src, const string& delim) {
  vector<string> elems;
  boost::split(elems, src, boost::is_any_of(delim));
  return elems;
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

inline vector<vector<string>> loadDelimitedFile(const string& path,
                                                const string& delimiter,
                                                int skip = 0) {
  vector<vector<string>> rows;
  auto lines = readFileLineByLine(path);
  rows.reserve(lines.size() - skip);
  for (int i = skip; i < lines.size(); i++) {
    auto elems = getSplit(lines[i], delimiter);
    rows.push_back(elems);
  }
  return rows;
}

inline vector<vector<string>> loadCsv(const string& path, int skip = 0) {
  return loadDelimitedFile(path, ",", skip);
}

inline vector<vector<string>> loadTsv(const string& path, int skip = 0) {
  return loadDelimitedFile(path, "\t", skip);
}

inline void saveDelimitedFile(const string& path,
                              const vector<vector<string>>& rows,
                              const string& delimiter) {
  ofstream ofs(path);
  for (const auto& row : rows) {
    for (size_t i = 0; i < row.size(); ++i) {
      ofs << row[i];
      if (i < row.size() - 1) ofs << delimiter;
    }
    ofs << "\n";
  }
  ofs.flush();
}

inline void saveCsv(const string& path, const vector<vector<string>>& rows) {
  saveDelimitedFile(path, rows, ",");
}

inline void saveTsv(const string& path, const vector<vector<string>>& rows) {
  saveDelimitedFile(path, rows, "\t");
}

}  // namespace utils
}  // namespace limas
