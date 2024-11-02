
#pragma once
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "geom/Mesh.h"

namespace limas {
namespace geom {
using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point2D = Kernel::Point_2;
using Point3D = Kernel::Point_3;
using Delaunay2D = CGAL::Delaunay_triangulation_2<Kernel>;
using Delaunay3D = CGAL::Delaunay_triangulation_3<Kernel>;

template <typename T>
inline std::vector<std::tuple<int, int, int>> delaunayTriangulation2D(
    const std::vector<T>& vertices) {
  Delaunay2D dt;
  std::unordered_map<Point2D, int> point_index_map;

  for (size_t i = 0; i < vertices.size(); ++i) {
    Point2D p(vertices[i].x, vertices[i].y);
    dt.insert(p);
    point_index_map[p] = i;
  }

  std::vector<std::tuple<int, int, int>> triangles;

  for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end();
       ++face) {
    int i1 = point_index_map[face->vertex(0)->point()];
    int i2 = point_index_map[face->vertex(1)->point()];
    int i3 = point_index_map[face->vertex(2)->point()];
    triangles.emplace_back(i1, i2, i3);
  }

  return triangles;
}

template <typename T>
inline std::vector<std::tuple<int, int, int, int>> delaunayTriangulation3D(
    const std::vector<T>& vertices) {
  Delaunay3D dt;
  std::unordered_map<Point3D, int> point_index_map;

  for (size_t i = 0; i < vertices.size(); ++i) {
    Point3D p(vertices[i].x, vertices[i].y, vertices[i].z);
    dt.insert(p);
    point_index_map[p] = i;
  }

  std::vector<std::tuple<int, int, int, int>> tetrahedrons;

  for (auto cell = dt.finite_cells_begin(); cell != dt.finite_cells_end();
       ++cell) {
    int i1 = point_index_map[cell->vertex(0)->point()];
    int i2 = point_index_map[cell->vertex(1)->point()];
    int i3 = point_index_map[cell->vertex(2)->point()];
    int i4 = point_index_map[cell->vertex(3)->point()];
    tetrahedrons.emplace_back(i1, i2, i3, i4);
  }

  return tetrahedrons;
}

template <typename T>
inline geom::Mesh delaunayTriangulation2DToMesh(
    const std::vector<T>& vertices) {
  auto triangles = delaunayTriangulation2D(vertices);
  geom::Mesh mesh;

  for (auto& v : vertices) {
    mesh.addVertex(glm::vec3(v.x, v.y, 0));
  }
  for (auto& t : triangles) {
    mesh.addIndex(std::get<0>(t));
    mesh.addIndex(std::get<1>(t));
    mesh.addIndex(std::get<2>(t));
  }
  return mesh;
}

}  // namespace geom
}  // namespace limas
