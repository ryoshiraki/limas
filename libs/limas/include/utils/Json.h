#pragma once
#include "graphics/Color.h"
#include "json.hpp"
#include "system/Exception.h"

namespace nlohmann {

// glm::vec2
template <typename T, glm::qualifier Q>
struct adl_serializer<glm::vec<2, T, Q>> {
  static void to_json(json& j, const glm::vec<2, T, Q>& v) {
    j = json{v.x, v.y};
  }

  static void from_json(const json& j, glm::vec<2, T, Q>& v) {
    v.x = j[0].get<T>();
    v.y = j[1].get<T>();
  }
};

// glm::vec3
template <typename T, glm::qualifier Q>
struct adl_serializer<glm::vec<3, T, Q>> {
  static void to_json(json& j, const glm::vec<3, T, Q>& v) {
    j = json{v.x, v.y, v.z};
  }

  static void from_json(const json& j, glm::vec<3, T, Q>& v) {
    v.x = j[0].get<T>();
    v.y = j[1].get<T>();
    v.z = j[2].get<T>();
  }
};

// glm::vec4
template <typename T, glm::qualifier Q>
struct adl_serializer<glm::vec<4, T, Q>> {
  static void to_json(json& j, const glm::vec<4, T, Q>& v) {
    j = json{v.x, v.y, v.z, v.w};
  }

  static void from_json(const json& j, glm::vec<4, T, Q>& v) {
    v.x = j[0].get<T>();
    v.y = j[1].get<T>();
    v.z = j[2].get<T>();
    v.w = j[3].get<T>();
  }
};

// glm::mat3
template <typename T, glm::qualifier Q>
struct adl_serializer<glm::mat<3, 3, T, Q>> {
  static void to_json(json& j, const glm::mat<3, 3, T, Q>& mat) {
    j = json::array();
    for (int i = 0; i < 3; ++i) {
      j.push_back(json{mat[i][0], mat[i][1], mat[i][2]});
    }
  }

  static void from_json(const json& j, glm::mat<3, 3, T, Q>& mat) {
    for (int i = 0; i < 3; ++i) {
      mat[i][0] = j[i][0].get<T>();
      mat[i][1] = j[i][1].get<T>();
      mat[i][2] = j[i][2].get<T>();
    }
  }
};

// glm::mat4
template <typename T, glm::qualifier Q>
struct adl_serializer<glm::mat<4, 4, T, Q>> {
  static void to_json(json& j, const glm::mat<4, 4, T, Q>& mat) {
    j = json::array();
    for (int i = 0; i < 4; ++i) {
      j.push_back(json{mat[i][0], mat[i][1], mat[i][2], mat[i][3]});
    }
  }

  static void from_json(const json& j, glm::mat<4, 4, T, Q>& mat) {
    for (int i = 0; i < 4; ++i) {
      mat[i][0] = j[i][0].get<T>();
      mat[i][1] = j[i][1].get<T>();
      mat[i][2] = j[i][2].get<T>();
      mat[i][3] = j[i][3].get<T>();
    }
  }
};

// Color
template <typename T>
struct adl_serializer<limas::BaseColor<T>> {
  static void to_json(json& j, const limas::BaseColor<T>& c) {
    j = json{c[0], c[1], c[2], c[3]};
  }

  static void from_json(const json& j, limas::BaseColor<T>& c) {
    c[0] = j[0].get<T>();
    c[1] = j[1].get<T>();
    c[2] = j[2].get<T>();
    c[3] = j[3].get<T>();
  }
};

}  // namespace nlohmann

using json = nlohmann::json;

namespace limas {
namespace utils {
  
inline json loadJson(const std::string& path) {
  std::ifstream ifs(path);
  if (!ifs) throw Exception("failed to load json from " + path);
  return json::parse(ifs);
}

inline void saveJson(json& j, const std::string& path) {
  std::ofstream ofs(path);
  if (!ofs) throw Exception("failed to write json to " + path);
  ofs << j;
}

}
}  // namespace limas
