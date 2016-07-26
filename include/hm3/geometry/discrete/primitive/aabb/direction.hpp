#pragma once
/// \file
///
/// Bounding box direction (1D).
#include <hm3/geometry/discrete/algorithm/direction.hpp>
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>
namespace hm3 {
namespace geometry {
namespace discrete {
namespace aabb_primitive {

/// Length of the 1D AABB \p s
constexpr vector<1> direction(aabb<1> const& s) noexcept {
  return vector<1>{x_max(s)(0) - x_min(s)(0)};
}

}  // namespace aabb_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
