#pragma once
/// \file
///
/// Does the AABB contains the point?
#include <hm3/geometry/discrete/algorithm/contains.hpp>
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>
namespace hm3 {
namespace geometry {
namespace discrete {
namespace aabb_primitive {

/// Does the AABB \p s contains the point \p p?
template <dim_t Nd>
constexpr bool contains(aabb<Nd> const& s, point<Nd> const& p) noexcept {
  return x_min(s) <= p and x_max(s) >= p;
}

}  // namespace aabb_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
