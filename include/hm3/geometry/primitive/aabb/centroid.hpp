#pragma once
/// \file
///
/// Centroid of an AABB.
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>

namespace hm3::geometry::aabb_primitive {

/// AABB centroid coordinates
template <dim_t Nd>
constexpr point<Nd> centroid(aabb<Nd> const& s) noexcept {
  return point<Nd>(x_min(s)() + .5 * (x_max(s)() - x_min(s)()));
}

}  // namespace hm3::geometry::aabb_primitive
