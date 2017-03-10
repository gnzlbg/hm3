#pragma once
/// \file
///
/// Ray concept.
#include <hm3/geometry/concept/line.hpp>

namespace hm3::geometry {

namespace concept {

struct Ray  // NOLINT(readability-identifier-naming)
 : rc::refines<GeometryObject, rc::Regular, LineLike> {
  template <typename T, typename UT = uncvref_t<T>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(
   rc::is_true(trait::check<UT, trait::ray<ambient_dimension_v<UT>>>), t.line()
   //
   ));
};

}  // namespace concept

/// Ray concept:
///
/// Refines: GeometryObject, Regular, LineLike
///
/// Requires:
///
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Ray = meta::and_<GeometryObject<T, Ad, 1>, rc::models<concept::Ray, T>>;

template <typename T, dim_t Ad = concept::detail::dimension_independent,
          typename UT = uncvref_t<T>>
using LineOrRay       = meta::and_<
 GeometryObject<UT, Ad, 1>,
 meta::or_<rc::models<concept::Ray, UT>, rc::models<concept::Line, UT>>>;

}  // namespace hm3::geometry
