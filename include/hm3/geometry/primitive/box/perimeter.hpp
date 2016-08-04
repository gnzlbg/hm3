#pragma once
/// \file
///
/// Box perimeter.
#include <hm3/geometry/algorithm/integral.hpp>
#include <hm3/geometry/primitive/box/bounding_length.hpp>
#include <hm3/geometry/primitive/box/box.hpp>
#include <hm3/utility/math.hpp>

namespace hm3::geometry::box_primitive {

/// Perimeter of a one-dimensional box.
///
/// In 1D it is always zero.
constexpr num_t perimeter(box<1> const&) noexcept { return math::eps; }

/// Perimeter of the box \p b.
///
/// In 2D this is the perimeter.
constexpr num_t perimeter(box<2> const& b) noexcept {
  return 4. * bounding_length(b, 0);
}

/// Perimeter of the box \p b.
///
/// In 3D this is the area of its faces.
constexpr num_t perimeter(box<3> const& b) noexcept {
  return 6. * math::ipow(bounding_length(b, 0), 2);
}

/// Surface area is a synonym for perimeter.
template <dim_t Nd>  //
constexpr num_t surface_area(box<Nd> const& s) noexcept {
  return perimeter(s);
}

}  // namespace hm3::geometry::box_primitive
