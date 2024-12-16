
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
inline std::vector<std::tuple<int, int, int>> getDelaunayTriangles2D(
    const std::vector<T>& vertices) {
  std::vector<std::tuple<int, int, int>> triangles;

  std::vector<Point2D> points(vertices.size());
  std::unordered_map<Point2D, int> point_index_map;
  for (size_t i = 0; i < vertices.size(); ++i) {
    Point2D p(vertices[i].x, vertices[i].y);
    points[i] = p;
    point_index_map[p] = i;
  }

  Delaunay2D dt;
  dt.insert(points.begin(), points.end());

  if (!dt.is_valid()) {
    logger::error("Delaunay triangulation is not valid");
    return triangles;
  }

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
inline std::vector<std::tuple<int, int>> getDelaunayEdges2D(
    const std::vector<T>& vertices) {
  std::vector<std::tuple<int, int>> edges;

  std::vector<Point2D> points(vertices.size());
  std::unordered_map<Point2D, int> point_index_map;
  for (size_t i = 0; i < vertices.size(); ++i) {
    Point2D p(vertices[i].x, vertices[i].y);
    // dt.insert(p);
    points[i] = p;
    point_index_map[p] = i;
  }

  Delaunay2D dt;
  dt.insert(points.begin(), points.end());

  if (!dt.is_valid()) {
    logger::error("Delaunay triangulation is not valid");
    return edges;
  }

  for (auto edge = dt.finite_edges_begin(); edge != dt.finite_edges_end();
       ++edge) {
    auto segment = dt.segment(*edge);

    int i1 = point_index_map[segment.source()];
    int i2 = point_index_map[segment.target()];
    edges.emplace_back(i1, i2);
  }

  return edges;
}

template <typename T>
inline std::vector<std::tuple<int, int, int, int>> getDelaunayTriangles3D(
    const std::vector<T>& vertices) {
  std::vector<std::tuple<int, int, int, int>> tetrahedrons;

  std::vector<Point3D> points(vertices.size());
  std::unordered_map<Point3D, int> point_index_map;
  for (size_t i = 0; i < vertices.size(); ++i) {
    Point3D p(vertices[i].x, vertices[i].y, vertices[i].z);
    // dt.insert(p);
    points[i] = p;
    point_index_map[p] = i;
  }

  Delaunay3D dt;
  dt.insert(points.begin(), points.end());

  if (!dt.is_valid()) {
    logger::error("Delaunay triangulation is not valid");
    return tetrahedrons;
  }

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
inline std::vector<std::tuple<int, int>> getDelaunayEdges3D(
    const std::vector<T>& vertices) {
  std::vector<std::tuple<int, int>> edges;

  std::vector<Point3D> points(vertices.size());
  std::unordered_map<Point3D, int> point_index_map;
  for (size_t i = 0; i < vertices.size(); ++i) {
    Point3D p(vertices[i].x, vertices[i].y, vertices[i].z);
    points[i] = p;
    point_index_map[p] = i;
  }

  Delaunay3D dt;
  dt.insert(points.begin(), points.end());
  if (!dt.is_valid()) {
    logger::error("Delaunay triangulation is not valid");
    return edges;
  }

  for (auto cell = dt.finite_edges_begin(); cell != dt.finite_edges_end();
       ++cell) {
    int i1 = point_index_map[cell->first->vertex(cell->second)->point()];
    int i2 = point_index_map[cell->first->vertex(cell->third)->point()];
    edges.emplace_back(i1, i2);
  }

  return edges;
}

}  // namespace geom
}  // namespace limas
