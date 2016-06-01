#pragma once
/// \file
///
/// AABB bounds
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/aabb/x_min_max.hpp>
#include <hm3/geometry/point/point.hpp>

namespace hm3 {
namespace geometry {

template <dim_t Nd> struct aabb_bounds { point<Nd> min, max; };

/// Bounds (min, max)
template <dim_t Nd>
constexpr aabb_bounds<Nd> bounds(aabb<Nd> const& s) noexcept {
  return {x_min(s), x_max(s)};
}

}  // namespace geometry
}  // namespace hm3
