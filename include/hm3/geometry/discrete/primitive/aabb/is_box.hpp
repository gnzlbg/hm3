#pragma once
/// \file
///
/// Is an AABB a Box
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>
#include <hm3/geometry/discrete/primitive/aabb/bounding_length.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace aabb_primitive {

/// Is the aabb \p s a box? (that is, all sides of \p s have the same length).
template <dim_t Nd>  //
constexpr bool is_box(aabb<Nd> const& s) {
  auto l = all_bounding_lengths(s);
  return ranges::all_of(l, [&](auto i) { return math::approx(i, l(0)); });
}

}  // namespace aabb_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
