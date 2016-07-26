#pragma once
/// \file
///
/// Bounding box length (1D).
#include <hm3/geometry/discrete/algorithm/length.hpp>
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>
#include <hm3/geometry/discrete/primitive/aabb/bounding_length.hpp>
namespace hm3 {
namespace geometry {
namespace discrete {
namespace aabb_primitive {

/// Length of the 1D AABB \p s
constexpr num_t length(aabb<1> const& s) noexcept {
  return bounding_length(s, 0);
}

}  // namespace aabb_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
