#pragma once
/// \file
///
/// Line concept.
#include <hm3/geometry/concept/geometry_object.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace concept {

struct LineLike  // NOLINT(readability-identifier-naming)
 : rc::refines<GeometryObject, rc::Regular> {
  template <typename T, typename UT = uncvref_t<T>,
            typename NT = associated::num_type_t<UT>,
            typename VT = associated::vector_t<UT>,  //
            typename PT = associated::point_t<UT>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(
   rc::convertible_to<PT>(t.x(NT{})),                              //
   rc::convertible_to<PT>(t.origin()),                             //
   rc::convertible_to<VT>(t.direction()),                          //
   rc::convertible_to<UT>(UT(/*origin*/ PT{}, /*dir*/ VT{})),      //
   rc::convertible_to<UT>(UT(/*point b*/ PT{}, /*point e*/ PT{}))  //
   //
   ));
};

struct Line  // NOLINT(readability-identifier-naming)
 : rc::refines<GeometryObject, rc::Regular, LineLike> {
  template <typename T, typename UT = uncvref_t<T>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(
   rc::is_true(trait::check<UT, trait::line<ambient_dimension_v<UT>>>)
   //
   ));
};

}  // namespace concept

template <typename T, dim_t Ad = concept::detail::dimension_independent>
using LineLike
 = meta::and_<GeometryObject<T, Ad, 1>, rc::models<concept::LineLike, T>>;

/// Line concept:
///
/// Refines: GeometryObject, Regular
///
/// Requires:
/// - t.x(num_t t) -> point_type: point at length t from the origin
/// - t.origin() -> point_type: origin of the line
/// - t.direction() -> vector_type: direction vector of the line
/// - T(point_t origin, vector_t direction)
/// - T(point_t begin, point_t end)
///
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Line = meta::and_<GeometryObject<T, Ad, 1>, rc::models<concept::Line, T>>;

}  // namespace hm3::geometry
