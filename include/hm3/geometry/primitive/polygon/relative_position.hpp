#pragma once
/// \file
///
/// Relative position of polygons with respect to other shapes
#include <hm3/geometry/algorith/intersection/polygon_sd.hpp>
#include <hm3/geometry/algorithm/relative_position.hpp>
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/sd/concept.hpp>

namespace hm3::geometry::polygon_primitive {

template <
 typename Shape, typename SDFunction,
 CONCEPT_REQUIRES_(Polygon<Shape>{}
                   and SignedDistance<SDFunction, dimension_v<Shape>{}>{})>
constexpr relative_position_t relative_position(Shape&& s,
                                                SDFunction&& sd) noexcept {
  auto intersected = interesction_test(s, sd);
  if (intersected) { return relative_position_t::intersected; }
  if (intersected.sign > 0) { return relative_position_t::inside; }
  return relative_position_t::outside;
}

}  // namespace hm3::geometry::polygon_primitive
