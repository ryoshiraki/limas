#pragma once
#include <map>

#include "geom/Mesh.h"

namespace rs {
namespace prim {

class IcoSphere : public geom::Mesh {
  struct Triangle {
    int ids[3];
    int operator[](int n) { return ids[n]; }
  };

  struct Edge {
    int ids[2];
    int operator[](int n) { return ids[n]; }
    bool operator<(const Edge& other) const {
      return std::tie(ids[0], ids[1]) < std::tie(other.ids[0], other.ids[1]);
    }
  };

 public:
  IcoSphere(float radius, unsigned int subdivisions, bool b_wireframe = false) {
    float t = (1.0f + glm::sqrt(5.0f)) / 2.0f;

    vertices_ = {{-1, t, 0}, {1, t, 0}, {-1, -t, 0}, {1, -t, 0},
                 {0, -1, t}, {0, 1, t}, {0, -1, -t}, {0, 1, -t},
                 {t, 0, -1}, {t, 0, 1}, {-t, 0, -1}, {-t, 0, 1}};

    for (glm::vec3& v : vertices_) {
      v = glm::normalize(v);
    }

    triangles_ = {{0, 11, 5}, {0, 5, 1},  {0, 1, 7},   {0, 7, 10}, {0, 10, 11},
                  {1, 5, 9},  {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
                  {3, 9, 4},  {3, 4, 2},  {3, 2, 6},   {3, 6, 8},  {3, 8, 9},
                  {4, 9, 5},  {2, 4, 11}, {6, 2, 10},  {8, 6, 7},  {9, 8, 1}};
    for (unsigned int i = 0; i < subdivisions; i++) {
      subdivide();
    }

    normals_ = vertices_;
    for (auto& v : vertices_) v *= radius;

    if (b_wireframe) {
      for (auto& t : triangles_) {
        for (int i = 0; i < 3; i++) {
          indices_.push_back(t.ids[i]);
          indices_.push_back(t.ids[(i + 1) % 3]);
        }
      }
    } else {
      for (auto& t : triangles_)
        for (int i = 0; i < 3; i++) {
          indices_.push_back(t.ids[i]);
        }
    }
  }

  const std::vector<Triangle>& getTriangles() const { return triangles_; };

 private:
  int vertexForedge(int v1, int v2) {
    Edge edge = {glm::min(v1, v2), glm::max(v1, v2)};
    auto it = edge_map_.find(edge);

    if (it != edge_map_.end()) {
      return it->second;
    } else {
      glm::vec3 new_vertex =
          glm::normalize(glm::vec3(vertices_[v1] + vertices_[v2]));
      vertices_.push_back(new_vertex);

      int new_index = vertices_.size() - 1;
      edge_map_[edge] = new_index;

      return new_index;
    }
  }

  void subdivide() {
    std::vector<Triangle> new_triangles;
    for (const Triangle& tri : triangles_) {
      int mid[3];
      for (int i = 0; i < 3; i++) {
        mid[i] = vertexForedge(tri.ids[i], tri.ids[(i + 1) % 3]);
      }

      new_triangles.push_back({tri.ids[0], mid[0], mid[2]});
      new_triangles.push_back({tri.ids[1], mid[1], mid[0]});
      new_triangles.push_back({tri.ids[2], mid[2], mid[1]});
      new_triangles.push_back({mid[0], mid[1], mid[2]});
    }

    triangles_ = new_triangles;
  }

  std::vector<Triangle> triangles_;
  std::map<Edge, int> edge_map_;
};

}  // namespace prim
}  // namespace rs
