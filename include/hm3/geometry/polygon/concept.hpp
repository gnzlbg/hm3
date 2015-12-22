#pragma once
/// \file
///
/// Polygon concept
///
/// Some types look like polygons but aren't (e.g. a 3D box).
/// The trait `is_polygon` can be specialized to false for those types.
#include <hm3/geometry/dimension/concept.hpp>

namespace hm3 {
namespace geometry {

namespace concepts {

template <typename T> struct is_polygon : std::true_type {};

namespace rc = ranges::concepts;

struct polygon : rc::refines<rc::Regular, Dimensional> {
  template <typename T>
  auto requires_(T&& t)
   -> decltype(rc::valid_expr(rc::model_of<rc::RandomAccessRange>(corners(t))),
               rc::is_true(is_polygon<rc::uncvref_t<T>>{}));
};

using Polygon = polygon;

}  // namespace concepts

template <typename T>
using Polygon = concepts::rc::models<concepts::Polygon, ranges::uncvref_t<T>>;

template <typename T, uint_t Nd>
using PolygonD
 = meta::and_c<concepts::rc::models<concepts::Polygon, ranges::uncvref_t<T>>{},
               NDimensional<T, Nd>{}>;

}  // namespace geometry
}  // namespace hm3
