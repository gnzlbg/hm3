#pragma once
/// \file
///
/// Bounding box of an any geometry
#include <hm3/geometry/algorithm/bounding_volume.hpp>
#include <hm3/geometry/primitive/any/any.hpp>

namespace hm3::geometry::any_primitive {

/// Square Bounding Box of the any geometry \p a.
template <dim_t Nd>
constexpr box<Nd> square_bounding_box(any<Nd> const a) noexcept {
  return visit([](auto&& i) { return bounding_volume.box(i); }, a);
}

/// Axis-Aligned Bounding Box of the any geometry \p a.
template <dim_t Nd>
constexpr aabb<Nd> axis_aligned_bounding_box(any<Nd> const& a) noexcept {
  return visit([](auto&& i) { return bounding_volume.aabb(i); }, a);
}

}  // namespace hm3::geometry::any_primitive
