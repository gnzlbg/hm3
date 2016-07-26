#pragma once
/// \file
///
/// Polygon concept
///
/// Some types look like polygons but aren't (e.g. a 3D box).
/// The trait `is_polygon` can be specialized to false for those types.
#include <hm3/geometry/dimension/concept.hpp>
#include <hm3/geometry/vertex_access.hpp>
#include <hm3/geometry/polygon/types.hpp>

namespace hm3 {
namespace geometry {

namespace concepts {

template <typename T> struct is_polygon : std::true_type {};

namespace rc = ranges::concepts;

struct polygon : rc::refines<rc::Regular, Dimensional> {
  template <typename T>
  static auto requires_(T&& t) -> decltype(                           //
   rc::valid_expr(                                                    //
    rc::is_true(is_polygon<rc::uncvref_t<T>>{}),                      //
    rc::model_of<rc::RandomAccessRange>(cdetail::vertices(t)),        //
    rc::model_of<rc::RandomAccessRange>(cdetail::vertex_indices(t)),  //
    cdetail::vertex(t, suint_t{0}),                                   //
    static_cast<bool>(cdetail::empty(t))                              //
    )                                                                 //
   );
};

using Polygon = polygon;

}  // namespace concepts

template <typename T>
using Polygon = concepts::rc::models<concepts::Polygon, ranges::uncvref_t<T>>;

template <typename T, dim_t Nd>
using PolygonD
 = meta::and_c<concepts::rc::models<concepts::Polygon, ranges::uncvref_t<T>>{},
               NDimensional<T, Nd>{}>;

}  // namespace geometry
}  // namespace hm3
