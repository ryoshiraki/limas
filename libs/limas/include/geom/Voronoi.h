
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
inline std::vector<VoronoiCell2D<T>> voronoiDiagram2D(
    const std::vector<T>& vertices) {
  std::vector<VoronoiCell2D<T>> cells;
  Voronoi2D vd;
  for (auto v : vertices) {
    vd.insert(Site2D(v.x, v.y));
  }

  if (!vd.is_valid()) {
    logger::error("Voronoi diagram is not valid");
    return cells;
  }

  for (int i = 0; i < vertices.size(); i++) {
    Point2D p(vertices[i].x, vertices[i].y);

    VD::Locate_result lr = vd.locate(p);
    // if (VD::Vertex_handle* v = std::get_if<VD::Vertex_handle>(&lr)) {
    // } else if (VD::Halfedge_handle* e =
    // std::get_if<VD::Halfedge_handle>(&lr)) { } else if (VD::Face_handle* f =
    // std::get_if<VD::Face_handle>(&lr)) {
    // }

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
inline geom::Mesh voronoiDiagram2DToEdgeMesh(const std::vector<T>& vertices) {
  geom::Mesh mesh;

  Voronoi2D vd;
  for (auto v : vertices) {
    vd.insert(Site2D(v.x, v.y));
  }

  if (!vd.is_valid()) {
    logger::error("Voronoi diagram is not valid");
    return mesh;
  }

  for (auto it = vd.edges_begin(); it != vd.edges_end(); ++it) {
    if (it->has_source() && it->has_target()) {
      auto source = it->source();
      auto target = it->target();
      mesh.addVertex(glm::vec3(source->point().x(), source->point().y(), 0));
      mesh.addVertex(glm::vec3(target->point().x(), target->point().y(), 0));
    }
  }

  return mesh;
}

}  // namespace geom
}  // namespace limas
