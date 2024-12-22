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

// glm::quat
template <typename T, glm::qualifier Q>
struct adl_serializer<glm::qua<T, Q>> {
  static void to_json(json& j, const glm::qua<T, Q>& q) {
    j = json{q.x, q.y, q.z, q.w};
  }

  static void from_json(const json& j, glm::qua<T, Q>& q) {
    q.x = j[0].get<T>();
    q.y = j[1].get<T>();
    q.z = j[2].get<T>();
    q.w = j[3].get<T>();
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
    if (j.is_array() && j.size() == 4) {
      for (int i = 0; i < 3; ++i) {
        if (j[i].is_array() && j[i].size() == 3) {
          mat[i][0] = j[i][0].get<T>();
          mat[i][1] = j[i][1].get<T>();
          mat[i][2] = j[i][2].get<T>();
        } else {
          throw limas::Exception("Invalid JSON format for glm::mat3");
        }
      }
    } else {
      throw limas::Exception("Invalid JSON format for glm::mat3");
    }
  }
};

// glm::mat4
template <typename T, glm::qualifier Q>
struct adl_serializer<glm::mat<4, 4, T, Q>> {
  static void to_json(json& j, const glm::mat<4, 4, T, Q>& mat) {
    j = json::array();
    for (int i = 0; i < 4; ++i) {
      j.push_back({mat[i][0], mat[i][1], mat[i][2], mat[i][3]});
    }
  }

  static void from_json(const json& j, glm::mat<4, 4, T, Q>& mat) {
    if (j.is_array() && j.size() == 4) {
      for (int i = 0; i < 4; ++i) {
        if (j[i].is_array() && j[i].size() == 4) {
          mat[i][0] = j[i][0].get<T>();
          mat[i][1] = j[i][1].get<T>();
          mat[i][2] = j[i][2].get<T>();
          mat[i][3] = j[i][3].get<T>();
        } else {
          throw limas::Exception("Invalid JSON format for glm::mat4");
        }
      }
    } else {
      throw limas::Exception("Invalid JSON format for glm::mat4");
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

template <typename BasicJsonType = json>
inline BasicJsonType loadJson(const std::string& path) {
  std::ifstream ifs(path);
  if (!ifs) throw Exception("failed to load json from " + path);
  return BasicJsonType::parse(ifs);
}

template <typename BasicJsonType>
inline void saveJson(const BasicJsonType& j, const std::string& path) {
  std::ofstream ofs(path);
  if (!ofs) throw Exception("failed to write json to " + path);
  ofs << j.dump(4);
}

}  // namespace utils
}  // namespace limas
