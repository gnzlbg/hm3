#pragma once
/// \file
///
/// Does the AABB contains the point?
#include <hm3/geometry/algorithm/contains.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
namespace hm3::geometry::aabb_primitive {

/// Does the AABB \p s contains the point \p p?
template <dim_t Nd>
constexpr bool contains(aabb<Nd> const& s, point<Nd> const& p) noexcept {
  return x_min(s) <= p and x_max(s) >= p;
}

}  // namespace hm3::geometry::aabb_primitive
