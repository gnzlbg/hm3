#pragma once
/// \file
///
/// AABB perimeter
#include <hm3/geometry/algorithm/perimeter.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/geometry/primitive/aabb/bounding_length.hpp>
#include <hm3/utility/math.hpp>

namespace hm3::geometry::aabb_primitive {

/// Perimeter of the axis-aligned bounding box.
///
/// In 1D it is always zero.
constexpr num_t perimeter(aabb<1> const&) noexcept { return math::eps; }

/// Perimeter of the axis-aligned bounding box \p b.
///
/// In 2D this is the perimeter.
constexpr num_t perimeter(aabb<2> const& b) noexcept {
  const auto ls = all_bounding_lengths(b);
  return 2. * (ls(0) + ls(1));
}

/// Perimeter of the axis-aligned bounding box \p b.
///
/// In 3D this is the area of its faces.
constexpr num_t perimeter(aabb<3> const& b) noexcept {
  const auto ls   = all_bounding_lengths(b);
  const auto lxly = ls(0) * ls(1);
  const auto lylz = ls(1) * ls(2);
  const auto lxlz = ls(0) * ls(2);
  return 2. * (lxly + lylz + lxlz);
}

/// Surface area is a synonym for perimeter.
template <dim_t Nd>  //
constexpr num_t surface_area(aabb<Nd> const& s) noexcept {
  return perimeter(s);
}

}  // namespace hm3::geometry::aabb_primitive
