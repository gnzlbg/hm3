#pragma once
/// \file
///
/// Plane concept.
#include <hm3/geometry/concept/associated_dimension.hpp>
#include <hm3/geometry/concept/associated_types.hpp>
#include <hm3/geometry/concept/fwd.hpp>
#include <hm3/geometry/concept/geometry_object.hpp>
#include <hm3/geometry/concept/primitive_hierarchy.hpp>

namespace hm3::geometry {

namespace concept {

struct Plane {
  template <typename T, typename UT = uncvref_t<T>,
            typename PT = associated::point_t<UT>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(
   rc::is_true(trait::check<UT, trait::plane<ambient_dimension_v<UT>>>),  //
   ((void)t.signed_distance(std::declval<PT>()), 42),                     //
   ((void)t.abs_distance(std::declval<PT>()), 42),                        //
   ((void)t.projection(std::declval<PT>()), 42),                          //
   ((void)t.distance_to_origin(), 42),                                    //
   ((void)t.normal(), 42)                                                 //
   ));
};

}  // namespace concept

/// Plane concept:
///
/// Refines: Edge, EdgeAccess
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Plane
 = meta::and_<GeometryObject<T, Ad,
                             Ad == concept::detail::dimension_independent
                              ? concept::detail::dimension_independent
                              : Ad - 1>,
              rc::models<concept::Plane, T>>;

}  // namespace hm3::geometry
