#pragma once
/// \file
///
/// Is an AABB a Box
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/aabb/centroid.hpp>
#include <hm3/geometry/aabb/length.hpp>
#include <hm3/geometry/point/point.hpp>

namespace hm3 {
namespace geometry {

/// Is the aabb spanned by \p x_min and \p x_max a box?
template <dim_t Nd> constexpr bool is_box(aabb<Nd> const& s) {
  auto l = lengths(s);
  return ranges::all_of(l, [&](auto i) { return math::approx(i, l(0)); });
}

}  // namespace geometry
}  // namespace hm3
