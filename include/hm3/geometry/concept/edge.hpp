#pragma once
/// \file
///
/// Edge concept.
#include <hm3/geometry/concept/geometry_object.hpp>
#include <hm3/geometry/concept/line.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/vertex_access.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace concept {

struct Edge  // NOLINT(readability-identifier-naming)
 : rc::refines<GeometryObject, rc::Regular, VertexAccess> {
  template <typename T, typename UT = uncvref_t<T>,
            typename VIT = associated::vertex_index_t<UT>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(
   rc::is_true(trait::check<UT, trait::segment<ambient_dimension_v<UT>>>),  //
   rc::model_of<LineLike>(t.line()),                                        //
   rc::model_of<Point>(uncvref_t<decltype(t.x(VIT{0}))>{})                  //
   ));
};

}  // namespace concept

/// Edge concept:
///
/// Refines: GeometryObject, Regular, VertexAccess
///
/// Requires:
/// - line() -> LineLike: unbounded line representation
///
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Edge = meta::and_<GeometryObject<T, Ad, 1>, rc::models<concept::Edge, T>>;

}  // namespace hm3::geometry
