#pragma once
/// \file
///
/// Merge polylines
#include <hm3/geometry/algorithm/set_union.hpp>
#include <hm3/geometry/primitive/polyline/set_union.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry::polyline_primitive {

template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<P, Nd>{})>
constexpr optional<UP> merge(P const& l0, P const& l1) {
  auto r = set_union(l0, l1);
  if (r) { return r; }
  return set_union(l0, direction.invert(l1));
}

template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<P, Nd>{})>
constexpr optional<UP> merge(P const& l0, segment<Nd> const& l1) {
  auto r = set_union(l0, l1);
  if (r) { return r; }
  return set_union(l0, direction.invert(l1));
}

}  // namespace hm3::geometry::polyline_primitive

namespace hm3::geometry {

using polyline_primitive::merge;

}  // namespace hm3::geometry
