#pragma once
/// \file
///
/// Simplifies polylines.
#include <hm3/geometry/algorithm/intersection/line_point.hpp>
#include <hm3/geometry/algorithm/set_union.hpp>
#include <hm3/geometry/primitive/polyline/set_union.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry::polyline_primitive {

template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<P, Nd>{})>
constexpr void simplify(P& l0) {
  for (suint_t fidx = 0; fidx < face_size(l0) - 1; ++fidx) {
    auto nfidx = fidx + 1;

    if (!intersection.test(face(l0, fidx).line(), face(l0, nfidx).x(1))) {
      continue;
    }
    vertex(l0, nfidx) = vertex(l0, nfidx + 1);
    l0.vertices().erase(begin(l0.vertices()) + nfidx + 1);
  }
}

}  // namespace hm3::geometry::polyline_primitive

namespace hm3::geometry {

using polyline_primitive::simplify;

}  // namespace hm3::geometry
