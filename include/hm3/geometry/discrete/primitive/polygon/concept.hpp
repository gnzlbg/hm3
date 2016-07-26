#pragma once
/// \file
///
/// Closed and Planar Polygon concept.
///
/// Some types look like polygons but aren't (e.g. a 2D line strip which isn't
/// closed). The trait `is_polygon` can be specialized to false for those types.
#include <hm3/geometry/discrete/concepts.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace concepts {

template <typename T> struct is_polygon : std::true_type {};

namespace rc = ranges::concepts;

struct polygon {  //}: rc::refines</*Primitive*/ concepts::dimensional> {
  template <typename T>
  static auto requires_(T&& t) -> decltype(      //
   rc::valid_expr(                               //
    rc::is_true(is_polygon<rc::uncvref_t<T>>{})  //,  //
    // rc::is_true(meta::bool_<rank(T{}) == 2>{}),   //
    // vertex_capacity(t)  //,                                 //
    // counter_clock_wise_order(t),                        //
    // clock_wise_order(t))                                //
    ));
};

using Polygon = polygon;

}  // namespace concepts

template <typename T, dim_t Nd = dimension_v<T>>
using Polygon
 = meta::and_<concepts::rc::models<concepts::Polygon, ranges::uncvref_t<T>>,
              meta::bool_<Dimensional<T, Nd>{}>>;

}  // namespace discrete

}  // namespace geometry
}  // namespace hm3
