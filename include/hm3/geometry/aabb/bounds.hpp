#pragma once
/// \file
///
/// AABB bounds
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/aabb/x_min_max.hpp>
#include <hm3/geometry/point/point.hpp>

namespace hm3 {
namespace geometry {

template <dim_t Nd>  //
struct aabb_bounds {
  aabb_bounds()
   : min(point<Nd>::constant(std::numeric_limits<num_t>::max()))
   , max(point<Nd>::constant(std::numeric_limits<num_t>::lowest())) {}
  aabb_bounds(point<Nd> xmin, point<Nd> xmax) : min(xmin), max(xmax) {
    HM3_ASSERT(valid(), "invalid aabb_bounds with xmin: {} and xmax: {}", min,
               max);
  }

  aabb_bounds(aabb_bounds const&) = default;
  aabb_bounds(aabb_bounds&&)      = default;
  aabb_bounds& operator=(aabb_bounds const&) = default;
  aabb_bounds& operator=(aabb_bounds&&) = default;

  /// Are the bounds valid? (min < max)!
  constexpr bool valid() noexcept {
    return (min().array() < max().array()).all();
  }

  point<Nd> min, max;
};

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& os, aabb_bounds<Nd> const& b) {
  os << "{min: " << b.min << ", max: " << b.max << "}";
  return os;
}

/// Bounds (min, max)
template <dim_t Nd>
constexpr aabb_bounds<Nd> bounds(aabb<Nd> const& s) noexcept {
  return {x_min(s), x_max(s)};
}

}  // namespace geometry
}  // namespace hm3
