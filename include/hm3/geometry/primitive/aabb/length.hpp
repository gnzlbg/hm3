#pragma once
/// \file
///
/// Bounding box length (1D).
#include <hm3/geometry/algorithm/length.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/geometry/primitive/aabb/bounding_length.hpp>
namespace hm3::geometry::aabb_primitive {

/// Length of the 1D AABB \p s
constexpr num_t length(aabb<1> const& s) noexcept {
  return bounding_length(s, 0);
}

}  // namespace hm3::geometry::aabb_primitive
