#pragma once
/// \file
///
/// Intersection between two points.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/point/point.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::point_primitive {

template <dim_t Nd>
constexpr bool intersection_test(point<Nd> const& a,
                                 point<Nd> const& b) noexcept {
  return a == b;
}

template <dim_t Nd>
constexpr variant<monostate, point<Nd>> intersection(
 point<Nd> const& a, point<Nd> const& b) noexcept {
  if (intersection_test(a, b)) { return a; }
  return {};
}

}  // namespace hm3::geometry::point_geometry
