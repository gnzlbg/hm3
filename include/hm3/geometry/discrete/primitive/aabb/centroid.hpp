#pragma once
/// \file
///
/// Centroid of an AABB.
#include <hm3/geometry/discrete/algorithm/centroid.hpp>
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>

namespace hm3 {
namespace geometry {

namespace discrete {
namespace aabb_primitive {

/// AABB centroid coordinates
template <dim_t Nd>  //
constexpr point<Nd> centroid(aabb<Nd> const& s) noexcept {
  return point<Nd>(x_min(s)() + .5 * (x_max(s)() - x_min(s)()));
}

}  // namespace aabb_primitive
}  // namespace discrete

}  // namespace geometry
}  // namespace hm3
