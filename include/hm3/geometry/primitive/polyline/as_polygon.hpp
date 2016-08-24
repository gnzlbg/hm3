#pragma once
/// \file
///
/// Conversion to a polygon.
#include <hm3/geometry/access/vertex.hpp>
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/primitive/polyline/faces.hpp>

namespace hm3::geometry {

namespace polyline_primitive {

/// Converts the polygon \p p into a polyline of type \tparam T .
template <typename T, typename P, typename UP = uncvref_t<P>,
          dim_t Nd = UP::dimension(), CONCEPT_REQUIRES_(Polyline<UP, Nd>{})>
constexpr T as_polygon(P&& p) {
  HM3_ASSERT(vertex_size(p) > 2,
             "polyline {} needs > 2 vertex to be a polygon");
  HM3_ASSERT(distance.approx(first_vertex(p), last_vertex(p)),
             "first and last polyline vertices must be equal: {} != {}",
             first_vertex(p), last_vertex(p));
  T result;
  result.reserve(vertex_size(p) - 1);
  for (auto&& v : vertices(p) | view::take(vertex_size(p) - 1)) {
    result.push_back(v);
  }
  return result;
}

}  // namespace polyline_primitive

using polyline_primitive::as_polygon;

}  // namespace hm3::geometry
