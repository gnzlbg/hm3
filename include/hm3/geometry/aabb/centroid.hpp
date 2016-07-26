#pragma once
/// \file
///
/// AABB centroid
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/aabb/bounds.hpp>
namespace hm3 {
namespace geometry {

namespace aabb_primitive {

/// AABB centroid coordinates
template <dim_t Nd>  //
constexpr point<Nd> centroid(aabb<Nd> const& s) noexcept {
  const auto b = bounds(s);
  return point<Nd>(b.min() + .5 * (b.max() - b.min()));
}

}  // namespace aabb_primitive

}  // namespace geometry
}  // namespace hm3
