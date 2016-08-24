#pragma once
/// \file
///
/// Conversion to a polyline.
#include <hm3/geometry/primitive/polygon/faces.hpp>

namespace hm3::geometry {

namespace polygon_primitive {

/// Converts the polygon \p p into a polyline of type \tparam T .
template <typename T, typename P, typename UP = uncvref_t<P>,
          dim_t Nd = UP::dimension(), CONCEPT_REQUIRES_(Polygon<UP, Nd>{})>
constexpr T as_polyline(P&& p) {
  HM3_ASSERT(vertex_size(p) > 1,
             "polygon {} needs > 1 vertex to be a polyline");
  T result;
  for (auto&& v : vertices(p)) { result.push_back(v); }
  result.push_back(vertex(p, 0));
  return result;
}

}  // namespace polygon_primitive

using polygon_primitive::as_polyline;

}  // namespace hm3::geometry
