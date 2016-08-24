#pragma once
/// \file
///
/// Does the box intersect a point?
#include <hm3/geometry/algorithm/contains.hpp>
#include <hm3/geometry/primitive/box/box.hpp>
namespace hm3::geometry::box_primitive {

/// Does the BOX \p s and the point \p p intersect?
template <dim_t Nd>
constexpr bool intersection_test(box<Nd> const& s,
                                 point<Nd> const& p) noexcept {
  return x_min(s) <= p and x_max(s) >= p;
}

/// Intersection between the box \p s and the point \p p.
template <dim_t Nd>
constexpr variant<monostate, point<Nd>> intersection(
 box<Nd> const& s, point<Nd> const& p) noexcept {
  if (intersection_test(s, p)) { return p; }
  return monostate{};
}

}  // namespace hm3::geometry::box_primitive
