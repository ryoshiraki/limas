#pragma once
#include "system/Exception.h"

namespace limas {
namespace util {

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

  if (cur.fail() || !cur.eof()) {
    throw Exception("Failed to convert string to the requested type: " + str);
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

}  // namespace util
}  // namespace limas
