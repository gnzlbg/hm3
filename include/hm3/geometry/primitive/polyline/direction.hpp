#pragma once
/// \file
///
/// Inverts the direction of a polyline.
#include <hm3/geometry/algorithm/direction.hpp>
#include <hm3/geometry/concepts.hpp>
#include <hm3/types.hpp>

namespace hm3::geometry::polyline_primitive {

/// Axis-Aligned Bounding Box of the polyline \p p.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<UP, Nd>{})>
constexpr P invert_direction(P p) {
  ranges::reverse(p);
  return p;
}

}  // namespace hm3::geometry::polyline_primitive
