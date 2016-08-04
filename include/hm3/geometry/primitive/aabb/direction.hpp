#pragma once
/// \file
///
/// Bounding box direction (1D).
#include <hm3/geometry/algorithm/direction.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
namespace hm3::geometry::aabb_primitive {

/// Length of the 1D AABB \p s
constexpr vec<1> direction(aabb<1> const& s) noexcept {
  return vec<1>{x_max(s)(0) - x_min(s)(0)};
}

}  // namespace hm3::geometry::aabb_primitive
