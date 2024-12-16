
#pragma once
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Voronoi_diagram_2.h>

#include "geom/Mesh.h"
#include "system/Logger.h"

namespace limas {
namespace geom {
using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using DT = CGAL::Delaunay_triangulation_2<K>;
using AT = CGAL::Delaunay_triangulation_adaptation_traits_2<DT>;
using AP = CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT>;
// using AP = CGAL::Delaunay_triangulation_adaptation_policies_2<DT>;
using VD = CGAL::Voronoi_diagram_2<DT, AT, AP>;

using Voronoi2D = CGAL::Voronoi_diagram_2<DT, AT, AP>;

using Site2D = AT::Site_2;
using Point2D = AT::Point_2;

template <typename T>
struct VoronoiCell2D {
  T site;
  std::vector<glm::vec2> vertices;
};

template <typename T>
inline std::vector<VoronoiCell2D<T>> getVoronoiCells2D(
    const std::vector<T>& vertices) {
  std::vector<VoronoiCell2D<T>> cells;

  std::vector<Point2D> points(vertices.size());
  for (size_t i = 0; i < vertices.size(); ++i) {
    Point2D p(vertices[i].x, vertices[i].y);
    points[i] = p;
  }

  Voronoi2D vd;
  vd.insert(points.begin(), points.end());

  if (!vd.is_valid()) {
    logger::error("Voronoi diagram is not valid");
    return cells;
  }

  for (int i = 0; i < points.size(); i++) {
    const auto& p = points[i];
    VD::Locate_result lr = vd.locate(p);

    if (VD::Face_handle* f = std::get_if<VD::Face_handle>(&lr)) {
      VoronoiCell2D<T> cell;
      cell.site = vertices[i];
      VD::Ccb_halfedge_circulator ec_start = (*f)->ccb();
      VD::Ccb_halfedge_circulator ec = ec_start;
      do {
        if (ec->has_source()) {
          cell.vertices.emplace_back(ec->source()->point().x(),
                                     ec->source()->point().y());
        }
      } while (++ec != ec_start);
      cells.push_back(cell);
    }
  }

  return cells;
}

template <typename T>
inline std::vector<std::tuple<glm::vec2, glm::vec2>> getVoronoiEdges2D(
    const std::vector<T>& vertices) {
  std::vector<std::tuple<glm::vec2, glm::vec2>> edges;

  std::vector<Point2D> points(vertices.size());

  for (size_t i = 0; i < vertices.size(); ++i) {
    Point2D p(vertices[i].x, vertices[i].y);
    points[i] = p;
  }

  Voronoi2D vd;
  vd.insert(points.begin(), points.end());

  if (!vd.is_valid()) {
    logger::error("Voronoi diagram is not valid");
    return edges;
  }

  for (auto it = vd.edges_begin(); it != vd.edges_end(); ++it) {
    if (it->has_source() && it->has_target()) {
      auto source = it->source();
      auto target = it->target();

      auto source_pt = glm::vec2(source->point().x(), source->point().y());
      auto target_pt = glm::vec2(target->point().x(), target->point().y());
      edges.emplace_back(source_pt, target_pt);
    }
  }

  return edges;
}

}  // namespace geom
}  // namespace limas
