#pragma once
/// \file
///
/// Does the AABB intersect a point?
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::aabb_primitive {

/// Does the AABB \p s and the point \p p intersect?
template <dim_t Nd>
constexpr bool intersection_test(aabb<Nd> const& s,
                                 point<Nd> const& p) noexcept {
  return x_min(s) <= p and x_max(s) >= p;
}

/// Intersection between the AABB \p s and the point \p p.
template <dim_t Nd>
constexpr variant<monostate, point<Nd>> intersection(
 aabb<Nd> const& s, point<Nd> const& p) noexcept {
  if (intersection_test(s, p)) { return p; }
  return monostate{};
}

}  // namespace hm3::geometry::aabb_primitive
